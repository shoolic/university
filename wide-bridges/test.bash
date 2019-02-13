#!/bin/bash
FILES=graphs/*
for f in $FILES
do
  echo $f:
  ./wide_bridges_finder < $f 
done