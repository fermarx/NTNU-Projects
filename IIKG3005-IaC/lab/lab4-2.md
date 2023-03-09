# Lab 4 - Puppet Modules - Part II

Write a Puppet module `gossinbackup` which installs and configures the code from <https://github.com/githubgossin/gossin-backup>. The module should allow the following parameters to be configured:
    
  |                         |                                                            |
  | :---------------------- | :--------------------------------------------------------- |
  | `source_directory`      | (REQUIRED)                                                 |
  | `destination_directory` | (REQUIRED)                                                 |
  | `exclude`               | (default `/opt/gossin-backup/rsyncexcludes`)               |
  | `days`                  | (how many daily backups to keep, default `7`)              |
  | `months`                | (how many monthly backups to keep, default `12`)           |
  | `years`                 | (how many yearly backups to keep default `3`)              |
  | `max_file_size`         | (default `100M`)                                           |
  | `exec_hours`            | (which hours this will execute, default every hour: `*/1`) |
  | `exec_minutes`          | (minutes of hour this will exec, default `[ 15, 45 ]`)     |
    

  Read [Module
  fundamentals](https://puppet.com/docs/puppet/latest/modules_fundamentals.html),
  use the [Beginner’s Guide to
  Modules](https://puppet.com/docs/puppet/latest/bgtm.html) and study
  the puppetlabs-ntp module mentioned there. Your module should have
  the following classes:
    
    - `gossinbackup`
    
    - `gossinbackup::install`
    
    - `gossinbackup::config`
    
  Your module should depend on the `puppetlabs-stdlib` so it can
  define some of the parameters as `Stdlib::Absolutepath` data type.
    
  Your module should depend on the `puppetlabs-vcsrepo` module and use
  it for installing the script by cloning the repo on GitHub.
    
  Your module should depend on the `puppetlabs-inifile` module and use
  it for editing the configuration file (`gossin-backup.conf`).
    
  Simplest possible use of your module should be:
    
      class { 'gossinbackup':
          source_directory      => '/home',
          destination_directory => '/backups',
      }


