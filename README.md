# c-mychmod
Unix/Linux  'chmod'  shell command implementation in C Language

In Unix/Linux, who can do what to a file or directory is controlled through sets of permissions. There are three sets of permissions. 
One set for the owner of the file, another set for the members of the file’s group, and a final set for everyone else. 

- r: Read permissions. The file can be opened, and its content viewed.
- w: Write permissions. The file can be edited, modified, and deleted.
- x: Execute permissions. If the file is a script or a program, it can be run (executed).

- u: User, meaning the owner of the file.
- g: Group, meaning members of the group the file belongs to.
- o: Others, meaning people not governed by the u and g permissions.
- a: All, meaning all of the above.

- –: Minus sign. Removes the permission.
- +: Plus sign. Grants the permission. The permission is added to the existing permissions. If you want to have this permission and only this permission set, use the = option, described below.
- =: Equals sign. Set a permission and remove others.

Custom 'mychmod' program's output:

```console
$ ./mychmod u+rwx,g=rw,o=r,+rx,+r data.txt 
$ ls -l data.txt 
-rwxrw-r--  1 batux  staff  0 Feb 28 22:35 data.txt
```
