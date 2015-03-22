#!/bin/sh

# Configurations
installPath="/vagrant/"
gitUserName="Spl3en"
gitEmail="spl3en.contact@gmail.com"



# === Installation starts from here ===

# Get packages
sudo apt-get update
sudo apt-get install g++
sudo apt-get install make
sudo apt-get install git
sudo apt-get install p7zip-full

# =========================================================
# =============            Git              ===============
# =========================================================

# Configure git
git config --global color.ui auto
git config --global user.name $gitUserName
git config --global user.email $gitEmail

# =========================================================
# =============           Redis             ===============
# =========================================================

# Get & Compile Redis environnement
wget http://download.redis.io/releases/redis-2.8.19.tar.gz
tar -xvf redis-2.8.19.tar.gz
cd redis-2.8.19/
make
sudo make install
cd $installPath
rm redis-2.8.19.tar.gz
rm -rf redis-2.8.19

# Get & Compile hiredis (C Client) environnement
wget https://github.com/redis/hiredis/archive/v0.12.1.tar.gz
tar -xvf v0.12.1.tar.gz
cd hiredis-0.12.1
make
sudo make install
cd $installPath
rm -rf hiredis-0.12.1
rm v0.12.1.tar.gz

# =========================================================
# =============            ZMQ              ===============
# =========================================================

# Get & Compile ZMQ environnement
wget http://download.zeromq.org/zeromq-4.1.0-rc1.tar.gz
tar -xvf zeromq-4.1.0-rc1.tar.gz
cd zeromq-4.1.0/
./configure
make
sudo make install
cd $installPath
rm -rf zeromq-4.1.0/
rm zeromq-4.1.0-rc1.tar.gz

# Get & Compile CZMQ environnement
wget http://download.zeromq.org/czmq-3.0.0-rc1.tar.gz
tar -xvf czmq-3.0.0-rc1.tar.gz
cd czmq-3.0.0/
./configure
make
sudo make install
cd $installPath
rm -rf czmq-3.0.0/
rm czmq-3.0.0-rc1.tar.gz


# =========================================================
# =============         CBP2MAKE          =================
# =========================================================
wget http://heanet.dl.sourceforge.net/project/cbp2make/cbp2make-stl-rev147-all.tar.7z
7za x cbp2make-stl-rev147-all.tar.7z
cd cbp2make-stl-rev147-all
make -f cbp2make.cbp.mak.unix
mkdir ~/.cbp2make
cp bin/Release/cbp2make ~/.cbp2make/
cd $installPath
rm -rf cbp2make-stl-rev147-all
rm cbp2make-stl-rev147-all.tar.7z

# =========================================================
# =============           R1EMU             ===============
# =========================================================

# Update ld
sudo ldconfig

# Compile R1EMU
cd $installPath
git clone https://github.com/rioru/treeofsavior.git
cd treeofsavior/server/R1EMU/projects/codeblocks/
~/.cbp2make/cbp2make -in R1EMU.workspace
mv R1EMU.workspace.mak Makefile
make clean
make