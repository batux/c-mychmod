# c-mychmod
Unix/Linux  'chmod'  shell command implementation in C Language

In Unix/Linux, who can do what to a file or directory is controlled through sets of permissions. There are three sets of permissions. 
One set for the owner of the file, another set for the members of the file’s group, and a final set for everyone else. 

- r: Read permissions. The file can be opened, and its content viewed.
- w: Write permissions. The file can be edited, modified, and deleted.
- x: Execute permissions. If the file is a script or a program, it can be run (executed).

POSIX 'mychmod' program's output:

```console
$ ./mychmod u+rwx,g=rw,o=r,+rx,+r data.txt 
$ ls -l data.txt 
-rwxrw-r--  1 batux  staff  0 Feb 28 22:35 data.txt
```
