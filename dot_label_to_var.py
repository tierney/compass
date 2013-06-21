#!/usr/bin/env python
import re

kRE_LABEL = re.compile("label=\"(\d+)\"\]")

var_to_label = {
0 : "attr(msg,courserating)",
1 : "attr(msg,discip)",
2 : "attr(msg,grades)",
3 : "attr(msg,tencase)",
4 : "inrole(p1,chair)",
5 : "inrole(p1,instructor)",
6 : "inrole(p1,student)",
7 : "inrole(p2,adboard)",
8 : "inrole(p2,admin)",
9 : "inrole(p2,factencom)",
10 : "subject(instructor)",
11 : "subject(student)",
12 : "subject(untenfac)",
}

with open('toss.dot') as fh:
  for line in fh:
    m = re.search(kRE_LABEL, line.strip())
    if m:
      new_label = var_to_label[int(m.groups()[0])]
      print re.sub(kRE_LABEL, "label=\"%s\"]" % new_label, line.strip())
    else:
      print line.strip()
