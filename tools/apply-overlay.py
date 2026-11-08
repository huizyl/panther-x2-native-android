#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Validate all fixed source inputs before applying any overlay changes."""
import argparse,hashlib,json,pathlib,shutil,subprocess
p=argparse.ArgumentParser();p.add_argument('--sdk',required=True,type=pathlib.Path);p.add_argument('--check',action='store_true');a=p.parse_args()
repo=pathlib.Path(__file__).resolve().parent.parent;sdk=a.sdk.resolve();j=json.loads((repo/'source-snapshot.json').read_text());plan=[]
def git(d,*args):return subprocess.run(['git','-C',str(d),*args],capture_output=True,text=True)
for row in j['changed_projects']:
 d=(sdk/row['path']).resolve();assert d.is_relative_to(sdk) and d.is_dir(),row['path']
 assert git(d,'rev-parse','HEAD').stdout.strip()==row['base_commit'],'Wrong base commit: '+row['path']
 patch=repo/'projects'/row['path']/'changes.patch';apply=False
 if patch.exists():
  if git(d,'apply','--check',str(patch)).returncode==0:apply=True
  else:assert git(d,'apply','--reverse','--check',str(patch)).returncode==0,'Patch conflicts: '+row['path']
 files=[]
 for item in row['untracked_source_files']:
  src=repo/'projects'/row['path']/'untracked'/item['path'];target=(d/item['path']).resolve()
  assert target.is_relative_to(d),'Unsafe source destination'
  assert hashlib.sha256(src.read_bytes()).hexdigest()==item['sha256'],'Source checksum mismatch'
  if target.exists():assert target.is_file() and target.read_bytes()==src.read_bytes(),'Existing file differs: '+str(target)
  else:files.append((src,target))
 plan.append((d,patch,apply,files))
print('Validated %d projects; no physical storage is accessed.'%len(plan))
if not a.check:
 for d,patch,apply,files in plan:
  if apply:subprocess.run(['git','-C',str(d),'apply',str(patch)],check=True)
  for src,target in files:target.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(src,target)
 print('Source overlay applied. Provision your own ADB public key before building.')
