#!/usr/bin/env sh
for f in stacks/*.c queues/*.c sequences/*.c maps/*.c ; do
	sh $f
done
