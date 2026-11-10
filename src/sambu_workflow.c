#include "sambu_workflow.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef enum TokenKind {
    TOK_EOF = 0, TOK_ID, TOK_STRING, TOK_NUMBER,
    TOK_LBRACE, TOK_RBRACE, TOK_EQUAL, TOK_DOT, TOK_SEMI
} TokenKind;

typedef struct Token {
    TokenKind kind;
    char text[SAMBU_WORKFLOW_TEXT_CAP];
    SambuSourceSpan span;
} Token;

typedef struct Parser {
    const char *source;
    size_t length;
    size_t offset;
    uint32_t line;
    uint32_t column;
    Token token;
    SambuWorkflowDiagnostic *diagnostic;
    bool failed;
} Parser;

static void copy_string(char *dst, size_t cap, const char *src)
{
    size_t n;
    if (!dst || cap == 0u) return;
    if (!src) src = "";
    n = strlen(src);
    if (n >= cap) n = cap - 1u;
    memcpy(dst, src, n);
    dst[n] = '\0';
}

static void set_error(Parser *p, SambuWorkflowError code,
                      SambuSourceSpan span, const char *message)
{
    if (!p || p->failed) return;
    p->failed = true;
    if (p->diagnostic) {
        p->diagnostic->code = code;
        p->diagnostic->source = span;
        copy_string(p->diagnostic->message,
                    sizeof(p->diagnostic->message), message);
    }
}

static void advance(Parser *p)
{
    if (p->offset >= p->length) return;
    if (p->source[p->offset] == '\n') { ++p->line; p->column = 1u; }
    else ++p->column;
    ++p->offset;
}

static void skip_space(Parser *p)
{
    while (p->offset < p->length && !p->failed) {
        char c = p->source[p->offset];
        if (isspace((unsigned char)c)) { advance(p); continue; }
        if (c == '#') {
            while (p->offset < p->length && p->source[p->offset] != '\n')
                advance(p);
            continue;
        }
        if (c == '/' && p->offset + 1u < p->length &&
            p->source[p->offset + 1u] == '/') {
            advance(p); advance(p);
            while (p->offset < p->length && p->source[p->offset] != '\n')
                advance(p);
            continue;
        }
        break;
    }
}

static void next_token(Parser *p)
{
    Token *t;
    char c;
    size_t n = 0u;
    if (!p || p->failed) return;
    skip_space(p);
    t = &p->token;
    memset(t, 0, sizeof(*t));
    t->span.offset = p->offset;
    t->span.line = p->line;
    t->span.column = p->column;
    if (p->offset >= p->length) { t->kind = TOK_EOF; return; }
    c = p->source[p->offset];
    switch (c) {
        case '{': t->kind = TOK_LBRACE; advance(p); break;
        case '}': t->kind = TOK_RBRACE; advance(p); break;
        case '=': t->kind = TOK_EQUAL; advance(p); break;
        case '.': t->kind = TOK_DOT; advance(p); break;
        case ';': t->kind = TOK_SEMI; advance(p); break;
        case '"':
            t->kind = TOK_STRING;
            advance(p);
            while (p->offset < p->length) {
                c = p->source[p->offset];
                if (c == '"') {
                    advance(p);
                    t->span.length = p->offset - t->span.offset;
                    t->text[n] = '\0';
                    return;
                }
                if (c == '\\') {
                    advance(p);
                    if (p->offset >= p->length) break;
                    c = p->source[p->offset];
                    if (c != '"' && c != '\\') {
                        set_error(p, SAMBU_WF_LEXICAL, t->span,
                                  "Only quote and slash escapes are supported.");
                        return;
                    }
                }
                if (n + 1u >= sizeof(t->text)) {
                    set_error(p, SAMBU_WF_SOURCE_LIMIT, t->span,
                              "String exceeds the fixed manifest capacity.");
                    return;
                }
                t->text[n++] = c;
                advance(p);
            }
            set_error(p, SAMBU_WF_LEXICAL, t->span,
                      "Unterminated string literal.");
            return;
        default:
            if (isdigit((unsigned char)c)) {
                t->kind = TOK_NUMBER;
                while (p->offset < p->length &&
                       isdigit((unsigned char)p->source[p->offset])) {
                    if (n + 1u >= sizeof(t->text)) {
                        set_error(p, SAMBU_WF_BUDGET, t->span,
                                  "Numeric token is too long.");
                        return;
                    }
                    t->text[n++] = p->source[p->offset];
                    advance(p);
                }
            } else if (isalpha((unsigned char)c) || c == '_') {
                t->kind = TOK_ID;
                while (p->offset < p->length) {
                    c = p->source[p->offset];
                    if (!isalnum((unsigned char)c) && c != '_' && c != '-') break;
                    if (n + 1u >= sizeof(t->text)) {
                        set_error(p, SAMBU_WF_SOURCE_LIMIT, t->span,
                                  "Identifier exceeds the fixed name capacity.");
                        return;
                    }
                    t->text[n++] = c;
                    advance(p);
                }
            } else {
                set_error(p, SAMBU_WF_LEXICAL, t->span,
                          "Unexpected character in workflow source.");
                return;
            }
            break;
    }
    t->text[n] = '\0';
    t->span.length = p->offset - t->span.offset;
}

static bool expect(Parser *p, TokenKind kind, const char *message)
{
    if (p->failed) return false;
    if (p->token.kind != kind) {
        set_error(p, SAMBU_WF_SYNTAX, p->token.span, message);
        return false;
    }
    next_token(p);
    return !p->failed;
}

static bool keyword(Parser *p, const char *word)
{
    if (p->token.kind != TOK_ID || strcmp(p->token.text, word) != 0) {
        set_error(p, SAMBU_WF_SYNTAX, p->token.span, "Unexpected keyword.");
        return false;
    }
    next_token(p);
    return !p->failed;
}

static bool name(Parser *p, char *dst, size_t cap)
{
    if (p->token.kind != TOK_ID) {
        set_error(p, SAMBU_WF_SYNTAX, p->token.span, "Expected identifier.");
        return false;
    }
    if (strlen(p->token.text) >= cap) {
        set_error(p, SAMBU_WF_SOURCE_LIMIT, p->token.span, "Identifier is too long.");
        return false;
    }
    copy_string(dst, cap, p->token.text);
    next_token(p);
    return !p->failed;
}

static SambuWorkflowAction resolve_action(const char *module, const char *verb)
{
    if (strcmp(module, "observe") == 0 && strcmp(verb, "apple_usb") == 0)
        return SAMBU_WF_SCAN_USB;
    if (strcmp(module, "observe") == 0 && strcmp(verb, "apple_service") == 0)
        return SAMBU_WF_READ_SERVICE;
    if (strcmp(module, "record") == 0 && strcmp(verb, "link_profile") == 0)
        return SAMBU_WF_RECORD_PROFILE;
    if (strcmp(module, "observe") == 0 && strcmp(verb, "backup_markers") == 0)
        return SAMBU_WF_AUDIT_BACKUP;
    if (strcmp(module, "compute") == 0 && strcmp(verb, "readiness") == 0)
        return SAMBU_WF_ASSESS_READINESS;
    return SAMBU_WF_NONE;
}

static bool parse_manifest(Parser *p, SambuWorkflow *out)
{
    unsigned seen = 0u;
    if (!keyword(p, "manifest") || !expect(p, TOK_LBRACE, "Expected manifest block."))
        return false;
    while (!p->failed && p->token.kind != TOK_RBRACE && p->token.kind != TOK_EOF) {
        char field[SAMBU_WORKFLOW_NAME_CAP];
        if (!name(p, field, sizeof(field))) return false;
        if (strcmp(field, "purpose") == 0) {
            if ((seen & 1u) || p->token.kind != TOK_STRING) {
                set_error(p, SAMBU_WF_SYNTAX, p->token.span, "Purpose must be one quoted string.");
                return false;
            }
            copy_string(out->purpose, sizeof(out->purpose), p->token.text);
            seen |= 1u;
            next_token(p);
        } else if (strcmp(field, "max_steps") == 0) {
            unsigned long value = 0u;
            size_t i;
            if (seen & 2u) {
                set_error(p, SAMBU_WF_SYNTAX, p->token.span, "max_steps is duplicated.");
                return false;
            }
            if (p->token.kind != TOK_NUMBER) {
                set_error(p, SAMBU_WF_SYNTAX, p->token.span, "max_steps must be an integer.");
                return false;
            }
            for (i = 0u; p->token.text[i]; ++i) {
                unsigned long digit = (unsigned long)(p->token.text[i] - '0');
                if (value > (SAMBU_WORKFLOW_STEP_CAP / 10u) ||
                    (value == (SAMBU_WORKFLOW_STEP_CAP / 10u) &&
                     digit > (SAMBU_WORKFLOW_STEP_CAP % 10u))) {
                    set_error(p, SAMBU_WF_BUDGET, p->token.span,
                              "max_steps is outside 1..8.");
                    return false;
                }
                value = value * 10u + digit;
            }
            if (value == 0u || value > SAMBU_WORKFLOW_STEP_CAP) {
                set_error(p, SAMBU_WF_BUDGET, p->token.span, "max_steps is outside 1..8.");
                return false;
            }
            out->max_steps = (uint16_t)value;
            seen |= 2u;
            next_token(p);
        } else if (strcmp(field, "autonomy") == 0) {
            if ((seen & 4u) || p->token.kind != TOK_ID) {
                set_error(p, SAMBU_WF_SYNTAX, p->token.span, "autonomy must be declared once.");
                return false;
            }
            if (strcmp(p->token.text, "observe_only") == 0)
                out->autonomy = SAMBU_WF_OBSERVE_ONLY;
            else if (strcmp(p->token.text, "local_record") == 0)
                out->autonomy = SAMBU_WF_LOCAL_RECORD;
            else {
                set_error(p, SAMBU_WF_POLICY, p->token.span, "Unknown autonomy class.");
                return false;
            }
            seen |= 4u;
            next_token(p);
        } else {
            set_error(p, SAMBU_WF_SYNTAX, p->token.span, "Unknown manifest field.");
            return false;
        }
        if (!expect(p, TOK_SEMI, "Expected ';' after manifest field.")) return false;
    }
    if (!expect(p, TOK_RBRACE, "Expected end of manifest.")) return false;
    if (seen != 7u || out->purpose[0] == '\0') {
        set_error(p, SAMBU_WF_SYNTAX, p->token.span,
                  "Manifest requires a non-empty purpose, max_steps and autonomy.");
        return false;
    }
    return true;
}

static bool parse_step(Parser *p, SambuWorkflow *out)
{
    SambuWorkflowStep step;
    char module[SAMBU_WORKFLOW_NAME_CAP] = {0};
    char verb[SAMBU_WORKFLOW_NAME_CAP] = {0};
    uint16_t i;
    memset(&step, 0, sizeof(step));
    step.source = p->token.span;
    if (!keyword(p, "step") || !name(p, step.name, sizeof(step.name)) ||
        !expect(p, TOK_EQUAL, "Expected '='.") ||
        !name(p, module, sizeof(module)) || !expect(p, TOK_DOT, "Expected qualified action.") ||
        !name(p, verb, sizeof(verb))) return false;
    step.action = resolve_action(module, verb);
    if (step.action == SAMBU_WF_NONE) {
        set_error(p, SAMBU_WF_UNKNOWN_ACTION, step.source,
                  "Action is not present in the ToolzNG host allowlist.");
        return false;
    }
    if (p->token.kind != TOK_SEMI) {
        set_error(p, SAMBU_WF_SYNTAX, p->token.span, "Expected ';' after step.");
        return false;
    }
    step.source.length = (p->token.span.offset + p->token.span.length) - step.source.offset;
    next_token(p);
    if (p->failed) return false;
    for (i = 0u; i < out->step_count; ++i) {
        if (strcmp(out->steps[i].name, step.name) == 0) {
            set_error(p, SAMBU_WF_DUPLICATE_STEP, step.source, "Step names must be unique.");
            return false;
        }
    }
    if (out->step_count >= SAMBU_WORKFLOW_STEP_CAP) {
        set_error(p, SAMBU_WF_BUDGET, step.source, "Compiled step capacity exceeded.");
        return false;
    }
    out->steps[out->step_count++] = step;
    return true;
}

bool sambu_workflow_compile(const char *source, size_t length,
                            SambuWorkflow *program,
                            SambuWorkflowDiagnostic *diagnostic)
{
    Parser p;
    if (diagnostic) memset(diagnostic, 0, sizeof(*diagnostic));
    if (!source || !program || length == 0u) {
        if (diagnostic) {
            diagnostic->code = SAMBU_WF_ARGUMENT;
            copy_string(diagnostic->message, sizeof(diagnostic->message), "Source and program are required.");
        }
        return false;
    }
    if (length > SAMBU_WORKFLOW_SOURCE_CAP) {
        if (diagnostic) {
            diagnostic->code = SAMBU_WF_SOURCE_LIMIT;
            copy_string(diagnostic->message, sizeof(diagnostic->message), "Workflow source exceeds 3072 bytes.");
        }
        return false;
    }
    memset(program, 0, sizeof(*program));
    memset(&p, 0, sizeof(p));
    p.source = source; p.length = length; p.line = 1u; p.column = 1u; p.diagnostic = diagnostic;
    next_token(&p);
    if (!keyword(&p, "workflow") || !name(&p, program->name, sizeof(program->name)) ||
        !expect(&p, TOK_LBRACE, "Expected workflow block.") || !parse_manifest(&p, program))
        return false;
    while (!p.failed && p.token.kind == TOK_ID && strcmp(p.token.text, "step") == 0) {
        if (!parse_step(&p, program)) return false;
    }
    if (!expect(&p, TOK_RBRACE, "Expected end of workflow.") || p.token.kind != TOK_EOF) {
        if (!p.failed) set_error(&p, SAMBU_WF_SYNTAX, p.token.span, "Unexpected trailing input.");
        return false;
    }
    if (program->step_count == 0u || program->step_count > program->max_steps) {
        if (diagnostic) {
            diagnostic->code = SAMBU_WF_BUDGET;
            copy_string(diagnostic->message, sizeof(diagnostic->message), "Empty workflow or manifest step budget exceeded.");
        }
        return false;
    }
    return true;
}

bool sambu_workflow_preflight(const SambuWorkflow *program,
                             const SambuWorkflowPolicy *policy,
                             SambuWorkflowDiagnostic *diagnostic)
{
    uint16_t i;
    if (diagnostic) memset(diagnostic, 0, sizeof(*diagnostic));
    if (!program || !policy) {
        if (diagnostic) {
            diagnostic->code = SAMBU_WF_ARGUMENT;
            copy_string(diagnostic->message, sizeof(diagnostic->message), "Program and policy are required.");
        }
        return false;
    }
    if (!program->step_count || program->max_steps == 0u ||
        program->max_steps > SAMBU_WORKFLOW_STEP_CAP ||
        program->step_count > program->max_steps ||
        program->step_count > policy->max_steps || program->step_count > SAMBU_WORKFLOW_STEP_CAP) {
        if (diagnostic) {
            diagnostic->code = SAMBU_WF_BUDGET;
            copy_string(diagnostic->message, sizeof(diagnostic->message), "Workflow exceeds the active step budget.");
        }
        return false;
    }
    if ((program->autonomy != SAMBU_WF_OBSERVE_ONLY &&
         program->autonomy != SAMBU_WF_LOCAL_RECORD) ||
        !policy->allow_observation || !sambu_authorize(SAMBU_OP_SCAN, SAMBU_ORIGIN_INTERNAL)) {
        if (diagnostic) {
            diagnostic->code = SAMBU_WF_POLICY;
            copy_string(diagnostic->message, sizeof(diagnostic->message), "Observation is not authorized.");
        }
        return false;
    }
    for (i = 0u; i < program->step_count; ++i) {
        const SambuWorkflowStep *step = &program->steps[i];
        if (!step->name[0]) {
            if (diagnostic) {
                diagnostic->code = SAMBU_WF_SYNTAX;
                diagnostic->source = step->source;
                copy_string(diagnostic->message, sizeof(diagnostic->message), "Step name is empty.");
            }
            return false;
        }
        if (step->action == SAMBU_WF_RECORD_PROFILE) {
            if (program->autonomy != SAMBU_WF_LOCAL_RECORD || !policy->allow_local_record ||
                !sambu_authorize(SAMBU_OP_RECORD_LOCAL_PROFILE, SAMBU_ORIGIN_INTERNAL)) {
                if (diagnostic) {
                    diagnostic->code = SAMBU_WF_POLICY;
                    diagnostic->source = step->source;
                    copy_string(diagnostic->message, sizeof(diagnostic->message), "Local profile recording failed dual preflight.");
                }
                return false;
            }
        } else if (step->action < SAMBU_WF_SCAN_USB || step->action > SAMBU_WF_ASSESS_READINESS) {
            if (diagnostic) {
                diagnostic->code = SAMBU_WF_UNKNOWN_ACTION;
                diagnostic->source = step->source;
                copy_string(diagnostic->message, sizeof(diagnostic->message), "Unrecognized action.");
            }
            return false;
        }
    }
    if (program->autonomy != SAMBU_WF_OBSERVE_ONLY &&
        program->autonomy != SAMBU_WF_LOCAL_RECORD) {
        if (diagnostic) diagnostic->code = SAMBU_WF_POLICY;
        return false;
    }
    return true;
}

bool sambu_workflow_begin(SambuWorkflowSession *session,
                          const SambuWorkflow *program,
                          const SambuWorkflowPolicy *policy,
                          const SambuWorkflowHost *host,
                          SambuWorkflowDiagnostic *diagnostic)
{
    if (!session || !host || !host->handler) {
        if (diagnostic) {
            memset(diagnostic, 0, sizeof(*diagnostic));
            diagnostic->code = SAMBU_WF_ARGUMENT;
            copy_string(diagnostic->message, sizeof(diagnostic->message), "A host action handler is required.");
        }
        return false;
    }
    memset(session, 0, sizeof(*session));
    if (!sambu_workflow_preflight(program, policy, diagnostic)) {
        session->state = SAMBU_WF_FAILED;
        if (diagnostic) session->run.diagnostic = *diagnostic;
        return false;
    }
    session->program = *program;
    session->policy = *policy;
    session->host = *host;
    session->run.preflight_approved = true;
    session->state = SAMBU_WF_RUNNING;
    return true;
}

static SambuWorkflowState workflow_session_invalid(
    SambuWorkflowSession *session, const char *message)
{
    session->state = SAMBU_WF_FAILED;
    session->run.completed = false;
    session->run.diagnostic.code = SAMBU_WF_SESSION_INVALID;
    memset(&session->run.diagnostic.source, 0,
           sizeof(session->run.diagnostic.source));
    copy_string(session->run.diagnostic.message,
                sizeof(session->run.diagnostic.message), message);
    return session->state;
}

SambuWorkflowState sambu_workflow_tick(SambuWorkflowSession *session)
{
    SambuWorkflowEvidence *entry;
    const SambuWorkflowStep *step;
    bool ok;
    if (session == NULL) return SAMBU_WF_FAILED;
    if (session->state != SAMBU_WF_RUNNING) return session->state;

    /* The session is public caller-owned state. Recheck its execution contract
       before indexing evidence or invoking a platform callback. */
    if (!session->run.preflight_approved || session->run.completed ||
        session->host.handler == NULL ||
        session->program.step_count == 0u ||
        session->program.step_count > SAMBU_WORKFLOW_STEP_CAP ||
        session->program.max_steps == 0u ||
        session->policy.max_steps == 0u ||
        session->program.max_steps > session->policy.max_steps ||
        session->program.step_count > session->program.max_steps ||
        session->policy.max_steps < session->program.step_count ||
        session->cursor >= session->program.step_count ||
        session->run.attempted != session->cursor ||
        session->run.failed != 0u ||
        session->run.passed != session->cursor ||
        session->run.attempted >= session->program.max_steps) {
        return workflow_session_invalid(
            session, "Workflow session invariants changed after preflight; no action was executed.");
    }
    for (uint16_t i = 0u; i < session->cursor; ++i) {
        const SambuWorkflowEvidence *prior = &session->run.evidence[i];
        if (prior->sequence != (uint16_t)(i + 1u) ||
            prior->action != session->program.steps[i].action ||
            !prior->succeeded) {
            return workflow_session_invalid(
                session, "Prior workflow evidence does not match the executed step sequence.");
        }
    }
    step = &session->program.steps[session->cursor];
    if (step->action < SAMBU_WF_SCAN_USB ||
        step->action > SAMBU_WF_ASSESS_READINESS ||
        memchr(step->name, '\0', sizeof(step->name)) == NULL ||
        step->name[0] == '\0') {
        return workflow_session_invalid(
            session, "Current workflow step is malformed or not allowlisted.");
    }
    if (step->action == SAMBU_WF_RECORD_PROFILE &&
        (session->program.autonomy != SAMBU_WF_LOCAL_RECORD ||
         !session->policy.allow_local_record ||
         !sambu_authorize(SAMBU_OP_RECORD_LOCAL_PROFILE, SAMBU_ORIGIN_INTERNAL))) {
        return workflow_session_invalid(
            session, "Current workflow step no longer satisfies local-record policy.");
    }
    entry = &session->run.evidence[session->cursor];
    memset(entry, 0, sizeof(*entry));
    entry->sequence = (uint16_t)(session->cursor + 1u);
    entry->action = step->action;
    entry->source = step->source;
    copy_string(entry->step_name, sizeof(entry->step_name), step->name);
    ++session->run.attempted;
    ok = session->host.handler(step->action, session->host.context,
                               entry->finding, sizeof(entry->finding));
    ++session->cursor;
    if (ok) {
        entry->succeeded = true;
        ++session->run.passed;
        if (!entry->finding[0]) copy_string(entry->finding, sizeof(entry->finding), "Completed.");
        if (session->cursor == session->program.step_count) {
            session->state = SAMBU_WF_COMPLETED;
            session->run.completed = true;
        }
    } else {
        ++session->run.failed;
        session->run.diagnostic.code = SAMBU_WF_HOST_FAILURE;
        session->run.diagnostic.source = step->source;
        copy_string(session->run.diagnostic.message, sizeof(session->run.diagnostic.message),
                    entry->finding[0] ? entry->finding : "Host action failed; remaining steps skipped.");
        session->state = SAMBU_WF_FAILED;
    }
    return session->state;
}

const char *sambu_workflow_action_name(SambuWorkflowAction action)
{
    switch (action) {
        case SAMBU_WF_SCAN_USB: return "observe.apple_usb";
        case SAMBU_WF_READ_SERVICE: return "observe.apple_service";
        case SAMBU_WF_RECORD_PROFILE: return "record.link_profile";
        case SAMBU_WF_AUDIT_BACKUP: return "observe.backup_markers";
        case SAMBU_WF_ASSESS_READINESS: return "compute.readiness";
        default: return "unknown";
    }
}

const char *sambu_workflow_error_name(SambuWorkflowError error)
{
    switch (error) {
        case SAMBU_WF_ARGUMENT: return "argument";
        case SAMBU_WF_SOURCE_LIMIT: return "source_limit";
        case SAMBU_WF_LEXICAL: return "lexical";
        case SAMBU_WF_SYNTAX: return "syntax";
        case SAMBU_WF_UNKNOWN_ACTION: return "unknown_action";
        case SAMBU_WF_DUPLICATE_STEP: return "duplicate_step";
        case SAMBU_WF_BUDGET: return "budget";
        case SAMBU_WF_POLICY: return "policy";
        case SAMBU_WF_HOST_FAILURE: return "host_failure";
        case SAMBU_WF_SESSION_INVALID: return "session_invalid";
        case SAMBU_WF_OK: default: return "ok";
    }
}

const char *sambu_workflow_state_name(SambuWorkflowState state)
{
    switch (state) {
        case SAMBU_WF_IDLE: return "idle";
        case SAMBU_WF_RUNNING: return "running";
        case SAMBU_WF_COMPLETED: return "completed";
        case SAMBU_WF_FAILED: return "failed";
        default: return "unknown";
    }
}
