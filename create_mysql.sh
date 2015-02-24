#!/bin/sh

echo "Please enter some parameters"
echo "============================"
echo -n "MySQL HOST (default localhost): "
read host
if [ "$host" = "" ]
then
    host="localhost"
fi
echo -n "MySQL USER (default root): "
read user
if [ "$user" = "" ]
then
    user="root"
fi
while [ "$password" = "" ]
do
    echo -n "MySQL PASSWORD: "
    read password
done
while [ "$db" = "" ]
do
    echo -n "MySQL DATABASE: "
    read db
done

echo ""
echo ""
echo "Creating database ..."
echo "====================="

mysql -v -h $host -u $user -p$password <<EOF
SET NAMES "utf8";
CREATE DATABASE IF NOT EXISTS \`$db\` DEFAULT COLLATE=utf8_general_ci DEFAULT CHARSET=utf8;
USE \`$db\`;
CREATE TABLE IF NOT EXISTS \`emeter\` (
  \`timestamp\` datetime NOT NULL,
  \`conskwh\` float NOT NULL,
  \`consws\` float NOT NULL,
  PRIMARY KEY (\`timestamp\`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
EOF

echo ""
echo "------------------------------------------------------"
echo "If no error appeared, database was successful created!"
