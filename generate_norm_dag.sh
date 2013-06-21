#!/bin/bash

./wiki_main
python dot_label_to_var.py > label.dot
dot -Tpdf label.dot > toss.pdf
