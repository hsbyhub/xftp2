project_dir=$(cd $(dirname $0); pwd)
data_dir=${project_dir}/data
build_dir=${project_dir}/build
xftp_home=/usr/local/xftp

echo "Into \"${build_dir}\" and build."
rm -rf ${build_dir}
cd ${build_dir}
mkdir build && cd build
cmake .. && make -j4
echo "Build done."

echo "Copy xftp server to \"${xftp_home}\""
rm -rf ${xftp_home}
mkdir -p ${xftp_home}
mkdir -p ${xftp_home}/bin
mkdir -p ${xftp_home}/root
cp -f xftp_server ${xftp_home}/bin
cp -f ${data_dir}/server.cfg ${xftp_home}
cp -f ${data_dir}/users.cfg ${xftp_home}

echo "Copy xftp.servive to \"/etc/systemd/system/\""
cp -f ${data_dir}/xftp.service /etc/systemd/system/
systemctl daemon-reload
systemctl enable xftp.service
systemctl start xftp.service

echo "xftp install and lauch done."
echo "xftp home is \"${xftp_home}\""
echo "Use \"systemctl\" to contral server, like \"systemctl status xftp.service\"."
echo "Thank you."