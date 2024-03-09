/usr/bin/bash

sed -e 's/href="\//href="/g' build/index.html  > build/a.html
mv build/a.html build/index.html
cd build
cp -R . /var/www/html/optquote

