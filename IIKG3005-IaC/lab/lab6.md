# Lab 6 - Control repository 

## Lab tutorial

Launch the Puppet learning-VM `Puppet Learning VM 2019.0.2 6.10` in
SkyHiGh and do the following quest:

      - Control Repository


In case you have not a server running the Puppet Learning VM, you 
need to create a new server using the image mentioned above. 
You find it in Glance, and choose a flavor with 2 vcpus.
    
To access the Quest Guide and get started, enter the learning-VM’s
IP address in your browser’s address bar (be sure to use
`http://<IPADDRESS>` for the Quest Guide, as `https://<IPADDRESS>`
will take you to the Puppet Enterprise console) or see
[Summary](https://github.com/puppetlabs/puppet-quest-guide/blob/master/summary.md)
    
You can log into the learning-VM with ssh as root, but you need to
access the console in Horizon to see the password on the splash page
(make a note of this password).


NOTE1. This quests assumes that you have done the previous ones, so in task 1-5 you 
are asked to copy some files that have been created in the previous quests to the 
new control repository. In case you don't have the files, I have prepared a repository 
containg the required files. You can get them by cloning the following repository into the Puppet
Learning VM:

      git clone https://gitlab.stud.idi.ntnu.no/jhklemet/puppetquest-control-repo.git

Then you just need to copy these files/folders into the control repository as described in the quest 
(note that you might need to change the source folder).

NOTE2. To access the web interface of the local Gitea repository on the Puppet Learning VM, you need to 
open port 3000 in the seurity group assigned to the server.

NOTE3. In task 17 you are asked to select the 'learning' user. In case you haven't done the previous 
quests, this user does not exist and you need to create it through the PE portal. From the menu to the left,
select 'SETUP->Access control->Users'. Fill inn 'learning' as the Full name and Login and click "Add local user'. 
Select 'SETUP->Access control->Users roles', and click on the 'Operators' link. Select the user 'learning' 
from the drop-down, and click 'Add user'. Press the 'Commit 1 change' button. 

You need also to set a new password for the 'learning' user. Go to 'SETUP->Access control->Users' and click 
on the user 'learning' in the list. Click on the link 'Generate password reset' in the upper right. Copy and open
the link in the pop-up window in a new browser window/tab. Set the new password to 'puppet'.

You can now proceed with task 17.



