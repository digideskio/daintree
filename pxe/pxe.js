var dgram = require('dgram');

if (!('SUDO_UID' in process.env)) {
  console.log('SUDO_UID not in process.env; don\'t know how to drop privileges');
  process.exit(1);
}

var s = dgram.createSocket('udp4');
s.bind(67, function() {
  process.setuid(parseInt(process.env.SUDO_UID));
  s.setBroadcast(true);
});

s.on('message', function(msg, rinfo) {
  var op = msg.readUInt8(0),
      htype = msg.readUInt8(1),
      hlen = msg.readUInt8(2),
      hops = msg.readUInt8(3),
      xid = msg.readUInt32BE(4),
      secs = msg.readUInt16BE(8),
      flags = msg.readUInt16BE(10),
      chaddr = msg.slice(28, 44),
      magic = msg.readUInt32BE(236);
  var options = msg.slice(240);

  if (magic != 0x63825363) {
    console.log('rejecting bad client; incorrect magic');
    return;
  }

  if (op != 1 || htype != 1 || hlen != 6 || hops != 0 || flags != 0x8000) {
    console.log('rejecting bad client: wrong op/htype/hlen/hops/flags');
    return;
  }

  // check options
  var optdict = {};
  for (var i = 0, ilen = options.length; i < ilen;) {
    var kind = options[i++];
    if (kind == 0) {
      continue;
    } else if (kind == 255) {
      break;
    }

    var len = options[i++];
    optdict[kind] = options.slice(i, i + len);
    i += len;
  }

  var requested = optdict[55];
  if (!requested) {
    console.log('doesn\'t look like pxe; no parameter request list');
    return;
  }

  var pxe = 0;  // need 8
  for (var j = 0, jlen = requested.length; j < jlen; ++j) {
    if (requested[j] >= 128 && requested[j] <= 135) {
      ++pxe;
    }
  }

  if (pxe != 8) {
    console.log('doesn\'t look like pxe; didn\'t request all pxe parameters');
  }

  var sysarch = optdict[93];
  if (!sysarch) {
    console.log('doesn\'t look like pxe; no sysarch');
    return;
  }

  if (sysarch.readUInt16BE(0) != 0x0000) {
    console.log('doesn\'t look like *our* pxe; wrong sysarch');
    return;
  }

  var nii = optdict[94];
  if (!nii) {
    console.log('doesn\'t look like pxe; no nii');
    return;
  }

  if (nii[0] != 0x01 || nii.readUInt16BE(1) != 0x0201) {
    console.log('doesn\'t look like *our* pxe; wrong nii');
    return;
  }

  var cmi = optdict[97];
  if (!cmi) {
    console.log('doesn\'t look like pxe; no cmi');
    return;
  }

  console.log('pxe request incoming: xid 0x' + xid.toString(16) + ', chaddr ' + chaddr.slice(0, hlen).toString('hex'));

  // offer some arbitrary IP for PXE
  //
  // assume we're 192.168.99.20, router/DNS's 192.168.99.1
  // it'll get 192.168.99.220 (0x0xc0a863dc)

  var response = new Buffer(336);
  response.writeUInt8(2, 0); // Boot Reply
  response.writeUInt8(1, 1); // ethernet
  response.writeUInt8(6, 2); // hw addr len
  response.writeUInt8(0, 3); // hops
  response.writeUInt32BE(xid, 4);
  response.writeUInt16BE(secs, 8);
  response.writeUInt16BE(0x8000, 10); // bootp flags
  response.writeUInt32BE(0, 12); // client IP
  response.writeUInt32BE(0xc0a863dc, 16); // your (client) IP
  response.writeUInt32BE(0xc0a86314, 20); // next server IP
  response.writeUInt32BE(0, 24); // relay IP
  chaddr.copy(response, 28);
  response.fill(0, 44, 108); // server host name
  response.write('dpxe', 44);
  response.fill(0, 108, 236); // boot file name
  response.write('/boot.pxe', 108);
  response.writeUInt32BE(0x63825363, 236); // magic

  response.writeUInt8(53, 240); // message type id
  response.writeUInt8(1, 241); // len
  response.writeUInt8(2, 242); // (boot reply)

  response.writeUInt8(54, 243); // dhcp server ID
  response.writeUInt8(4, 244); // len
  response.writeUInt32BE(0xc0a86314, 245); // this IP

  response.writeUInt8(51, 249); // lease time
  response.writeUInt8(4, 250); // len
  response.writeUInt32BE(864000, 251); // 10 days

  response.writeUInt8(58, 255); // renewal time value
  response.writeUInt8(4, 256); // len
  response.writeUInt32BE(432000, 257); // 5 days

  response.writeUInt8(59, 261); // rebinding time value
  response.writeUInt8(4, 262); // len
  response.writeUInt32BE(756000, 263); // 8 days, 18 hours

  response.writeUInt8(1, 267); // subnet mask
  response.writeUInt8(4, 268); // len
  response.writeUInt32BE(0xffffff00, 269); // /24

  response.writeUInt8(3, 273); // router
  response.writeUInt8(4, 274); // len
  response.writeUInt32BE(0xc0a86301, 275); // 192.168.99.1

  response.writeUInt8(6, 279); // DNS server
  response.writeUInt8(4, 280); // len
  response.writeUInt32BE(0xc0a86301, 281); // 192.168.99.1

  response.writeUInt8(15, 285); // domain name
  response.writeUInt8(8, 286); // len
  response.write('daintree', 287); // ibid

  response.writeUInt8(28, 295); // broadcast address
  response.writeUInt8(4, 296); // len
  response.writeUInt32BE(0xc0a863ff, 297); // 192.168.99.255

  response.writeUInt8(42, 301); // NTP server addresses
  response.writeUInt8(4, 302); // len
  response.writeUInt32BE(0xc0a86301, 303); // 192.168.99.1

  // echo 93, 94, 97
  response.writeUInt8(93, 307); // client sysarch
  response.writeUInt8(2, 308); // len
  sysarch.copy(response, 309);

  response.writeUInt8(94, 311); // nii
  response.writeUInt8(3, 312); // len
  nii.copy(response, 313);

  response.writeUInt8(97, 316); // cmi
  response.writeUInt8(17, 317); // len
  cmi.copy(response, 318);

  response.writeUInt8(255, 335); // end!

  s.send(response, 0, 336, 68, '0.0.0.0', function(err, bytes) {
    console.log('sent; err ' + err + ', bytes ' + bytes);
  });
});
