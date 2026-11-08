#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Provision only explicitly selected Android ADB public keys, never private keys."""
import argparse,base64,pathlib,struct
p=argparse.ArgumentParser();p.add_argument('--sdk',required=True,type=pathlib.Path);p.add_argument('--public-key',required=True,type=pathlib.Path,action='append');a=p.parse_args()
keys=[]
for file in a.public_key:
 for line in file.read_text().splitlines():
  if not line.strip():continue
  raw=base64.b64decode(line.split()[0],validate=True)
  assert len(raw)==524 and struct.unpack_from('<I',raw)[0]==64,'Expected Android ADB RSA public key, not an SSH key or private key'
  if line.strip() not in keys:keys.append(line.strip())
assert keys
d=a.sdk.resolve()/'device/rockchip/rk356x/rk3566_panther_x2/adb-authorized';d.mkdir(parents=True,exist_ok=True)
target=d/'adb_keys';assert not target.exists(),'Refusing to overwrite existing ADB authorization'
target.write_text('\n'.join(keys)+'\n');target.chmod(0o644)
print('Provisioned %d explicitly selected public keys. Do not distribute the configured image.'%len(keys))
