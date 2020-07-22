# Contributing

Here are the steps for adding a feature to this system.  
(All commands are done in a git bash terminal. Sourcetree instructions haven't been included)

## Contents
1. [Make sure that your local master branch is up to date](#local-up-to-date)
2. [Rebasing the feature branch off master](#rebase-feature)
3. [Adding a new feature](#when-new-feature)  
    a. [Make a new feature branch](#new-feature-branch)  
        i. [Make a new  branch from terminal](#new-branch-terminal)
4. [While editing](#editing)
5. [When the new feature is finished](#new-feature-finished)  
    a. [Make a pull request](#make-pull-request)
6. [When the feature branch is ready to be merged](#ready-for-merge)  
    a. [Update the branches](#update-branches)  
    b. [Squash the commits](#squash-commits)  
    c. [Action the merge in stash](#action-merge)
7. [Useful commands](#useful-commands)
    
    
    
<a name = "local-up-to-date"></a>
## Make sure that your local master branch is up to date
Update your local master branch often (or whenever someone has merged a new commit to master).

To update master, perform the following steps in the root of the project repository

````
$ git fetch  

$ git checkout master

$ git pull
````

(If you are merging to master, return to step 2 of [When the feature branch is ready to be merged](#ready-for-merge) to continue)

<a name = "rebase-feature"></a>
## Rebasing the feature branch off master

Rebasing is an alternative to merging where all the new commits are recommitted over the
existing branch, resulting in a linear commit history.



````
$ git checkout <BRANCH TO BE REBASED> 

$ git rebase origin/master
(if required, resolve any merge conflicts)

Use tig here to check if the rebase has worked:
$ tig 

Push the changes:
$ git push -f origin $(git rev-parse --abbrev-ref HEAD)
````

<a name = "when-new-feature"></a>
## Adding a new feature
 
<a name = "new-feature-branch"></a>
### Make a new feature branch

Each branch should contain the changes for one new feature.
The id for the branches are not automated, so it's better to make the branch on the stash website.

````    
$ git fetch

$ git checkout [new branch name]
````
<a name = "new-branch-terminal"></a>
#### Make a new branch from terminal

It may be easier to make a new branch from the terminal.  
Make sure you are in the root directory of the repository, on the master branch.

````
$ git checkout -b [new branch name]-[NEXT NUMBER] - [new-feature-branch]
````

The first time you push the new branch, use

````
$ git push -u origin [new branch name]
```` 
<a name = "editing"></a>
## While editing

Git add/commit/push every time you have made a substantial change.
Use the commit message to document what change you have made. Reviewers will be able to see your 
commit history, so they can use the commit messages to see what code goes to which change.

````
Make sure you are on your feature branch:
$ git branch 
(Your branch should start with a *)

$ git add .

$ git commit -m "Useful message"

$ git push
````

<a name = "new-feature-finished"></a>
## When the new feature is finished (or so you think)

<a name = "ready-for-merge"></a>  
## When the feature branch is ready to be merged 

<a name = "update-branches"></a>
#### Update the branches
1. Follow the steps in "[Make sure your local branch is up to date](#local-up-to-date)"
2. <a name = "squash-commits"></a> Squash your commits before rebasing and force-pushing your changes.

    (Squashing all your changes into one commit means that you won't clog up the master branch with 
    unnecessary commits.)
 

        $ git checkout <BRANCH TO BE MERGED> 
        
        $ git reset --soft $(git merge-base HEAD origin/master)
        
        $ git commit -am "WEA-<XX> - <MEANINGFUL MESSAGE>"
        
        $ git rebase origin/master
        (if required, resolve any merge conflicts)
         
        $ git push -f origin $(git rev-parse --abbrev-ref HEAD)

    
   You can use [tig](#useful-commands) to check that all your commits are on top of the exiting commits in the master branch

3.  <a name = "action-merge"></a> Action the merge in Stash:
    1.  Ensure that the commit message has the correct number and that the content of the message is in the correct format, e.g. "WEA-7 - added CONTRIBUTING.md file"
    2.  If this is not the case go through the steps in the paragraph "Squash the commits " again to change the commit message
    3.  Review the pull request in Stash to check if all the changes are there and if only a single commit is listed.
    4.  **Tick the checkbox "Delete source branch after merging"** and click on the merge button. 
 
<a name = "useful-commands"></a>
## Useful commands


````
To check the state of your working directory and staging area:
$ git status

If another user has pushed to your feature branch and you wish to rebase with these changes:
$ git pull --rebase

To check your local commit history:
$ tig
(press 'q' to exit tig)

Show what has changed since the last commit:
$ git diff HEAD

Show what has changed but hasn't been added to the index yet via git add:
$ git diff

To reset the branch back to the remote version:
$ git reset --hard origin/[branch name]
(this will delete all the changes that haven't been pushed)

To check which branch you're on:
$ git branch
````
