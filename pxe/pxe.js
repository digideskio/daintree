var dgram = 'dgram';

if (!('SUDO_UID' in process.env)) {
    console.log('SUDO_UID not in process.env; don\'t know how to drop privileges');
    process.exit(1);
}

var Bootp = {
    options: {
        PAD: 0,
        SUBNET_MASK: 1,
        ROUTER: 3,
        DNS_SERVER: 6,
        DOMAIN_NAME: 15,
        BROADCAST: 28,
        NTP_SERVERS: 42,
        MESSAGE_TYPE: 53,
        SERVER_ID: 54,
        PARAMETER_REQUEST_LIST: 55,
        RENEWAL_TIME: 58,
        REBINDING_TIME: 59,
        PXE_SYSARCH: 93,
        PXE_NII: 94,
        PXE_CMI: 97,
        END: 255
    },
    MAGIC: 0x63825363,
    FLAGS_BROADCAST: 0x8000,
    read: function (msg) {
        if (msg.readUInt32BE(236) != Bootp.MAGIC) {
            return null;
        }

        var r = {
            op: msg.readUInt8(0),
            htype: msg.readUInt8(1),
            hlen: msg.readUInt8(2),
            hops: msg.readUInt8(3),
            xid: msg.readUInt32BE(4),
            secs: msg.readUInt16BE(8),
            flags: msg.readUInt16BE(10),
            chaddr: msg.slice(28, 44),
            options: {}
        };

        var options = msg.slice(240);

        for (var i = 0, ilen = options.length; i < ilen; ) {
            var kind = options[i++];
            if (kind == 0) {
                continue;
            } else if (kind == 255) {
                break;
            }

            var len = options[i++];
            r.options[kind] = options.slice(i, i + len);
            i += len;
        }

        return r;
    }
};

var s = dgram.createSocket('udp4');
s.bind(67, function () {
    process.setuid(parseInt(process.env.SUDO_UID));
    s.setBroadcast(true);
});

s.on('message', function (msg, rinfo) {
    var r = readBootp(msg);
    if (r.op != 1 || r.htype != 1 || r.hlen != 6 || r.hops != 0 || r.flags != Bootp.FLAGS_BROADCAST) {
        console.log('rejecting bad client: wrong op/htype/hlen/hops/flags');
        return;
    }

    var requested = r.options[55];
    if (!requested) {
        console.log('doesn\'t look like pxe; no parameter request list');
        return;
    }

    var pxe = 0;
    for (var j = 0, jlen = requested.length; j < jlen; ++j) {
        if (requested[j] >= 128 && requested[j] <= 135) {
            ++pxe;
        }
    }

    if (pxe != 8) {
        console.log('doesn\'t look like pxe; didn\'t request all pxe parameters');
    }

    var sysarch = r.options[93];
    if (!sysarch) {
        console.log('doesn\'t look like pxe; no sysarch');
        return;
    }

    if (sysarch.readUInt16BE(0) != 0x0000) {
        console.log('doesn\'t look like *our* pxe; wrong sysarch');
        return;
    }

    var nii = r.options[94];
    if (!nii) {
        console.log('doesn\'t look like pxe; no nii');
        return;
    }

    if (nii[0] != 0x01 || nii.readUInt16BE(1) != 0x0201) {
        console.log('doesn\'t look like *our* pxe; wrong nii');
        return;
    }

    var cmi = r.options[97];
    if (!cmi) {
        console.log('doesn\'t look like pxe; no cmi');
        return;
    }

    console.log('pxe request incoming: xid 0x' + xid.toString(16) + ', chaddr ' + chaddr.slice(0, hlen).toString('hex'));

    var response = new Buffer(336);
    response.writeUInt8(2, 0);
    response.writeUInt8(1, 1);
    response.writeUInt8(6, 2);
    response.writeUInt8(0, 3);
    response.writeUInt32BE(xid, 4);
    response.writeUInt16BE(secs, 8);
    response.writeUInt16BE(Bootp.FLAGS_BROADCAST, 10);
    response.writeUInt32BE(0, 12);
    response.writeUInt32BE(0xc0a863dc, 16);
    response.writeUInt32BE(0xc0a86314, 20);
    response.writeUInt32BE(0, 24);
    chaddr.copy(response, 28);
    response.fill(0, 44, 108);
    response.write('dpxe', 44);
    response.fill(0, 108, 236);
    response.write('/boot.pxe', 108);
    response.writeUInt32BE(Bootp.MAGIC, 236);

    response.writeUInt8(53, 240);
    response.writeUInt8(1, 241);
    response.writeUInt8(2, 242);

    response.writeUInt8(54, 243);
    response.writeUInt8(4, 244);
    response.writeUInt32BE(0xc0a86314, 245);

    response.writeUInt8(51, 249);
    response.writeUInt8(4, 250);
    response.writeUInt32BE(864000, 251);

    response.writeUInt8(58, 255);
    response.writeUInt8(4, 256);
    response.writeUInt32BE(432000, 257);

    response.writeUInt8(59, 261);
    response.writeUInt8(4, 262);
    response.writeUInt32BE(756000, 263);

    response.writeUInt8(1, 267);
    response.writeUInt8(4, 268);
    response.writeUInt32BE(0xffffff00, 269);

    response.writeUInt8(3, 273);
    response.writeUInt8(4, 274);
    response.writeUInt32BE(0xc0a86301, 275);

    response.writeUInt8(6, 279);
    response.writeUInt8(4, 280);
    response.writeUInt32BE(0xc0a86301, 281);

    response.writeUInt8(15, 285);
    response.writeUInt8(8, 286);
    response.write('daintree', 287);

    response.writeUInt8(28, 295);
    response.writeUInt8(4, 296);
    response.writeUInt32BE(0xc0a863ff, 297);

    response.writeUInt8(42, 301);
    response.writeUInt8(4, 302);
    response.writeUInt32BE(0xc0a86301, 303);

    response.writeUInt8(93, 307);
    response.writeUInt8(2, 308);
    sysarch.copy(response, 309);

    response.writeUInt8(94, 311);
    response.writeUInt8(3, 312);
    nii.copy(response, 313);

    response.writeUInt8(97, 316);
    response.writeUInt8(17, 317);
    cmi.copy(response, 318);

    response.writeUInt8(255, 335);

    s.send(response, 0, 336, 68, '0.0.0.0', function (err, bytes) {
        console.log('sent; err ' + err + ', bytes ' + bytes);
    });
});

