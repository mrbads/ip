#!/bin/sh

export JAVA_HOME=/usr/local/java
PATH=/usr/local/java/bin:${PATH}

THE_CLASSPATH=
for i in 'ls ./lib/*.jar'
do
  THE_CLASSPATH=${THE_CLASSPATH}:${i}
done

javac Syn1.java
javac Syn2.java
