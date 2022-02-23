# Define util function
function EchoRed() {
    echo -e "\033[1;31m${*}\033[0m"
}
function EchoGreen() {
    echo -e "\033[1;32m${*}\033[0m"
}
function EchoPurple() {
    echo -e "\033[1;35m${*}\033[0m"
}

# Set path var
project_dir=$(cd $(dirname $0); pwd)
install_dir=/usr/local
xftp_home=${install_dir}/xftp

# Goto build directory
cd ${project_dir}
rm -rf build
mkdir build
cd build
EchoGreen "In ${project_dir}/build"

# Install libxco
git clone https://gitee.com/hsby/libxco
cd libxco
chmod a+x install.sh
./install.sh
EchoGreen "Install libxco done."

# Build xftp
cd ${project_dir}/build
cmake .. && make -j4
EchoGreen "Build xftp done."

# Install xftp
cd ${project_dir}
cp -rf xftp ${install_dir}
EchoGreen "Install xftp done."

# Install xftp.service
cd ${project_dir}
cp -f xftp.service /etc/systemd/system/
EchoGreen "Install xftp.service done."

# Start xftp.service
systemctl daemon-reload
systemctl enable xftp.service
systemctl start xftp.service
systemctl status xftp
EchoGreen "Start xftp.service done."

# Echo some tips
echo ""
EchoPurple "All xftp install program is done."
EchoPurple "Tips:"
EchoPurple "-Default [Home]=\"${xftp_home}\""
EchoPurple "-Default [Share Diretory]=\"${xftp_home}/root/\""
EchoPurple "-Default [Config Diretory]=\"${xftp_home}/config/\""
EchoPurple "-Use \"systemctl [operation] xftp\" to contral server."
EchoPurple "Thank you."