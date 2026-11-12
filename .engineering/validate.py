#!/usr/bin/env python3
"""Validate the repository-local Engineering Standard profile (stdlib only)."""
from __future__ import annotations
import json, os, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
PROFILE=ROOT/".engineering"/"profile.json"
STANDARD=ROOT/"ENGINEERING_STANDARD.md"
GATES={"profile-validation","native-project-checks","architecture-and-compatibility-review","security-and-privacy-review","documentation-review","human-review","target-qualification-when-applicable"}
STATUSES={"PROPOSED","PLANNED","IMPLEMENTED","TESTED","VERIFIED","MERGED","RELEASED","BLOCKED"}
def main():
    errors=[]
    for path,label in ((PROFILE,".engineering/profile.json"),(STANDARD,"ENGINEERING_STANDARD.md"),(ROOT/"README.md","README.md")):
        if not path.is_file(): errors.append("missing "+label)
    if errors:
        print("\n".join("FAIL: "+x for x in errors),file=sys.stderr); return 1
    try: d=json.loads(PROFILE.read_text(encoding="utf-8"))
    except (OSError,UnicodeError,json.JSONDecodeError) as exc:
        print(f"FAIL: invalid profile JSON: {exc}",file=sys.stderr); return 1
    if d.get("schema_version")!=1: errors.append("schema_version must equal 1")
    if d.get("standard_id")!="PERSONA-ENG-STD": errors.append("invalid standard_id")
    repo=d.get("repository")
    if not isinstance(repo,str) or "/" not in repo: errors.append("repository must be owner/name")
    expected=os.environ.get("GITHUB_REPOSITORY")
    if expected and repo!=expected: errors.append(f"repository {repo!r} does not match {expected!r}")
    project=d.get("project")
    if not isinstance(project,dict): errors.append("project must be an object"); project={}
    for k in ("domain","mission","maturity"):
        if not isinstance(project.get(k),str) or not project[k].strip(): errors.append(f"project.{k} must be non-empty")
    arch=d.get("architecture")
    if not isinstance(arch,dict): errors.append("architecture must be an object"); arch={}
    if arch.get("preserve_existing_boundaries") is not True: errors.append("preserve_existing_boundaries must be true")
    for k in ("primary_technologies","explicit_non_goals"):
        v=arch.get(k)
        if not isinstance(v,list) or not v or any(not isinstance(x,str) or not x.strip() for x in v): errors.append(f"architecture.{k} must be a non-empty string array")
    safety=d.get("safety_boundaries")
    if not isinstance(safety,list) or not safety or any(not isinstance(x,str) or not x.strip() for x in safety): errors.append("safety_boundaries must be a non-empty string array")
    native=d.get("native_validation")
    if not isinstance(native,dict): errors.append("native_validation must be an object"); native={}
    if native.get("status") not in {"defined","discovery-required","target-evidence-pending"}: errors.append("invalid native_validation.status")
    commands=native.get("commands")
    if not isinstance(commands,list) or any(not isinstance(x,str) or not x.strip() for x in commands): errors.append("native_validation.commands must be an array of command strings")
    gates=d.get("required_gates")
    if not isinstance(gates,list): errors.append("required_gates must be an array"); gates=[]
    if GATES-set(gates): errors.append("missing required gates: "+", ".join(sorted(GATES-set(gates))))
    merge=d.get("merge_policy")
    if not isinstance(merge,dict): errors.append("merge_policy must be an object"); merge={}
    if merge.get("pull_request_required") is not True: errors.append("pull_request_required must be true")
    if merge.get("bypass_required_checks") is not False: errors.append("bypass_required_checks must be false")
    if merge.get("release_requires_separate_qualification") is not True: errors.append("release qualification must remain separate")
    statuses=d.get("status_vocabulary")
    if not isinstance(statuses,list) or not STATUSES.issubset(set(statuses)): errors.append("status_vocabulary must include all standard statuses")
    if errors:
        print("\n".join("FAIL: "+x for x in errors),file=sys.stderr); return 1
    print(f"ENGINEERING STANDARD PASS: {repo}")
    print("Governance metadata validated; native builds, tests, reviews, and target qualification remain separate.")
    return 0
if __name__=="__main__": raise SystemExit(main())
