# ft_traceroute

recode the command line traceroute to work with ipv4 and ipv6

## options
* [-f first_ttl]
* [-m max_ttl]
* [-q nqueries]
* [-w waittime]
* [-n]

## linux

* Use the following command to avoid permissions.
sudo setcap cap_net_raw+ep ft_traceroute