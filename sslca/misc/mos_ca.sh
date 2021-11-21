#!/bin/bash
# Author: MOS
# Script name: mos_ca.sh
# Date & Time: 2013-01-06/23:05:35
# Version: 1.0.2
# Description:
#
[ -f /etc/sysconfig/mos_ca.conf ] && . /etc/sysconfig/mos_ca.conf
Config(){
Cnf=${Cnf:-/usr/lib/ssl/openssl.cnf}
cp $Cnf $Cnf.`date +%F-%T`.bak
Dir=${Dir:-/etc/pki/CA/}
Cny=${Cny:-CN}
Pve=${Pve:-Henan}
Cty=${Cty:-Zhengzhou}
Bis=${Bis:-Youguess}
Bnh=${Bnh:-Tech}
OPN=`grep "stateOrProvinceName_default.*=[[:space:]].*" /usr/lib/ssl/openssl.cnf|cut -d"t" -f1`
sed -i "s@dir.∗=[[:space:]].*@\1$Dir@g" $Cnf
sed -i "s@countryNamedefault.∗=[[:space:]].*@\1$Cny@g" $Cnf
    if [ $OPN = s ];then
        sed -i "s@stateOrProvinceNamedefault.∗=[[:space:]].*@\1$Pve@g" $Cnf
    else
        sed -i "s@^#stateOrProvinceNamedefault.∗=[[:space:]].*@\1$Pve@g" $Cnf
    fi
sed -i "s@localityNamedefault.∗=[[:space:]].*@\1$Cty@g" $Cnf
sed -i "s@0.organizationNamedefault.∗=[[:space:]].*@\1$Bis@g" $Cnf
sed -i "s@^#organizationalUnitNamedefault\([[:space:]]=\)@\1 $Bnh@g" $Cnf
}

Create_CA(){
Dir=${Dir:-/etc/pki/CA/}
[ ! -d ${Dir}crl ] && mkdir -pm 700 ${Dir}crl
[ ! -d ${Dir}newcerts ] && mkdir -pm 700 ${Dir}newcerts
[ ! -d ${Dir}certs ] && mkdir -pm 700 ${Dir}certs
[ ! -f ${Dir}index.txt ] && touch ${Dir}index.txt
[ ! -f ${Dir}serial ] && echo 01 > ${Dir}serial
[ ! -d ${Dir}private ] && mkdir -pm 700 ${Dir}private
(umask 077; openssl genrsa -out ${Dir}private/cakey.pem 2048 &> /dev/null )
#read -p "Please input CA hostname [default:ca.mos.com]: " Host
Host=${Host:-ca.mos.com}
#read -p "Please input CA E-mail [default]:root@mos.com]: " Em
Em=${Em:-root@mos.com}
echo -e "\n\n\n\n\n${Host}\n${Em}\n"|openssl req -x509 -new -key ${Dir}private/cakey.pem -out ${Dir}cacert.pem -days 3650 &> /dev/null
}

Create(){
Dir=${Dir:-/etc/pki/CA/}
Date=`date +%F-%H:%M:%S`
[ ! -f /etc/pki/CA/private/cakey.pem ] && Create_CA && return 0
if [ -f /etc/pki/CA/private/cakey.pem ];then
    read -p "CA existe,Continue? y: Move file; n-> Quit. [y|n] " Choice
    if [[ "$Choice" == "y" ]];then
        [ ! -d "$Dir""$Date"tmp ] && mkdir -p "$Dir""$Date"tmp
            mv "$Dir"* "$Dir""$Date"tmp/ &> /dev/null
            Create_CA
    elif [[ "$Choice" == "n" ]];then
        exit 0
    else
        echo "Error input..."
        exit 1
    fi
fi
}

Create_crt(){
(umask 077;openssl genrsa 1024 > "$Ddir""$Dname".key) &> /dev/null
echo -e "\n\n\n\n\n"$Dhost"\n"$DE"\n\n\n"|openssl req -new -key "$Ddir""$Dname".key -out "$Ddir""$Dname".csr &> /dev/null
[[ $Set != "-s" ]] && echo -e "y\ny\n"|openssl ca -in "$Ddir""$Dname".csr -out "$Ddir""$Dname".crt -days 365 &> /dev/null && exit 0
[[ $Set == "-s" ]] && exit 0
}

Demo_crt(){
Ddir=${Ddir:-/opt/mos_demo/}
Dname=${Dname:-demo}
Dhost=${Dhost:-demo.mos.com}
De=${De:-root@mos.com}
Date=`date +%F-%H:%M:%S`
[ ! -d $Ddir ] && mkdir -p $Ddir &> /dev/null
[[ ! -f "$Ddir""$Dname".key && ! -f "$Ddir""$Dname".csr ]] && Create_crt && exit 0
if [[ -f "$Ddir""$Dname".key || -f "$Ddir""$Dname".csr || -f "$Ddir""$Dname".crt ]];then
    read -p "Demo certificate existe, Continue? y: Move file; n: Quit. [y|n] " Cie
    if [[ "$Cie" == "y" ]];then
[ ! -d "$Ddir""$Date"tmp ] && mkdir -p "$Ddir""$Date"tmp && mv "$Ddir""$Dname"* "$Ddir""$Date"tmp/ &> /dev/null
        Create_crt
        exit 0
    elif [[ "$Cie" == "n" ]];then
        exit 0
    else
        echo "Error input..."
    fi
fi    
}

Set=$1
if [[ $Set  =~ (-n)|(-o)|(-s)|(CA) && -z $2 || -z $Set ]];then 
    [ -z $Set ] && Config && Create && Demo_crt && exit 0
    [ $Set = -n ] && Demo_crt && exit 0 
    [ $Set = -s ] && Config && Demo_crt && exit 0 
    [ $Set = CA ] && Config && Create && exit 0 
    [ $Set = -o ] && echo -e "y\ny\n"|openssl ca -in "$Other".csr -out "$Other".crt -days 365 &> /dev/null
else
    echo "Error,Invalid option!"
    exit 1
fi