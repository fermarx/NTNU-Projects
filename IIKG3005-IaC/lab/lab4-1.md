# Lab 4 - Puppet Modules - Part I

## Lab tutorial

1.  Create a new stack with an Ubuntu instance that has Puppet installed. You can use the [following tutorial.](https://gitlab.stud.idi.ntnu.no/jhklemet/iikg3005-labs/-/blob/master/puppet_vm.md)

2.  Install the `puppetlabs-ntp` and `saz-timezone` modules and write a
    manifest `time.pp` which declares ntp and configures timezone to be
    `Europe/Oslo`. Apply the manifest with `puppet apply` (apply it
    repeatedly until it has converged, meaning it only generates the
    minimum output every time you apply it). Run the commands as root.
    
        puppet module install puppetlabs-ntp
        puppet module install saz-timezone
      
      time.pp:
        
        node default {
          include ntp
          class { 'timezone':
            timezone => 'Europe/Oslo',
          }
        }

3.  Install the modules `puppetlabs-apache` and `hunner-wordpress` and
    write a manifest that installs wordpress (according to the README
    at <https://github.com/hunner/puppet-wordpress>)
    
        puppet module install puppetlabs-apache
        puppet module install hunner-wordpress
        puppet module list --tree
        
      wordpress.pp:
        
        node default {
        
          $mysql_password        = 'put_in_hiera'
          $wordpress_password    = 'put_also_in_hiera'
        
          # PHP-enabled web server
          class { 'apache':
            default_vhost => false,
            mpm_module    => 'prefork',
          }
          class { 'apache::mod::php':
              php_version => '7.2',
          }
        
          # Virtual Host for Wordpress
          apache::vhost { $::fqdn:
            port           => 80,
            docroot        => '/opt/wordpress',
            manage_docroot => false,
          }
        
          # MySQL server
          class { 'mysql::server':
            root_password => $mysql_password,
          }
          class { 'mysql::bindings':  # if added late, need to restart apache service
            php_enable => true,
            php_package_name => 'php-mysql',
          }
        
          # Wordpress
          user { 'wordpress':
            ensure => 'present'
          }
          class { 'wordpress':
            version     => '4.9.7',   # if changed, delete index.php, rerun puppet
                                      # due to creates => "${install_dir}/index.php"
                                      # in manifests/instance/app.pp
            wp_owner    => 'wordpress',
            wp_group    => 'wordpress',
            db_user     => 'wordpress',
            db_password => $wordpress_password,
            require     => [ Class['apache'], Class['mysql::server'], User['wordpress'] ],
          }
        }

## Review questions and problems

1.  Give a brief overview of how you can test Puppet modules (from the
    simplest to the most advanced testing).

2.  Install the module `puppetlabs-motd`. See the [README file](https://forge.puppet.com/puppetlabs/motd) for examples of how
    to use it. Write a Puppet manifest where you where you declare the class `motd` and
    let the parameter `template` be `'motd/motd.epp'`  
    
    Do a `puppet apply`, log out and back in to see the result. Btw, where is the file
    `motd/motd.epp` in the file system?
