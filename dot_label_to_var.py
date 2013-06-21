#!/usr/bin/env python
import re

kRE_LABEL = re.compile("label=\"(\d+)\"\]")

from vars import var_to_label

with open('toss.dot') as fh:
  for line in fh:
    m = re.search(kRE_LABEL, line.strip())
    if m:
      new_label = var_to_label[int(m.groups()[0])]
      print re.sub(kRE_LABEL, "label=\"%s\"]" % new_label, line.strip())
    else:
      print line.strip()
