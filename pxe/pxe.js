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

  var response = new Buffer(329);
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
  response.fill(0, 108, 236); // boot file name
  response.writeUInt32BE(0x63825363, 236); // magic

  response.writeUInt8(53, 237); // message type id
  response.writeUInt8(2, 238); // (boot reply)

  response.writeUInt8(54, 239); // dhcp server ID
  response.writeUInt8(4, 240); // len
  response.writeUInt32BE(0xc0a86314, 241); // this IP

  response.writeUInt8(51, 245); // lease time
  response.writeUInt8(4, 246); // len
  response.writeUInt32BE(864000, 247); // 10 days

  response.writeUInt8(58, 251); // renewal time value
  response.writeUInt8(4, 252); // len
  response.writeUInt32BE(432000, 253); // 5 days

  response.writeUInt8(59, 257); // rebinding time value
  response.writeUInt8(4, 258); // len
  response.writeUInt32BE(756000, 259); // 8 days, 18 hours

  response.writeUInt8(1, 263); // subnet mask
  response.writeUInt8(4, 264); // len
  response.writeUInt32BE(0xffffff00, 265); // /24

  response.writeUInt8(3, 269); // router
  response.writeUInt8(4, 270); // len
  response.writeUInt32BE(0xc0a86301, 271); // 192.168.99.1

  response.writeUInt8(6, 275); // DNS server
  response.writeUInt8(4, 276); // len
  response.writeUInt32BE(0xc0a86301, 277); // 192.168.99.1

  response.writeUInt8(15, 281); // domain name
  response.writeUInt8(8, 282); // len
  response.write('daintree', 283); // ibid

  response.writeUInt8(28, 291); // broadcast address
  response.writeUInt8(4, 292); // len
  response.writeUInt32BE(0xc0a863ff, 293); // 192.168.99.255

  response.writeUInt8(28, 297); // NTP server addresses
  response.writeUInt8(4, 298); // len
  response.writeUInt32BE(0xc0a86301, 299); // 192.168.99.1

  // echo 93, 94, 97
  response.writeUInt8(93, 300); // client sysarch
  response.writeUInt8(2, 301); // len
  sysarch.copy(response, 302);

  response.writeUInt8(94, 304); // nii
  response.writeUInt8(3, 305); // len
  nii.copy(response, 306);

  response.writeUInt8(97, 309); // cmi
  response.writeUInt8(17, 310); // len
  cmi.copy(response, 311);

  response.writeUInt8(0, 328); // end!

  s.send(response, 0, 329, 68, '0.0.0.0', function(err, bytes) {
    console.log('sent; err ' + err + ', bytes ' + bytes);
  });
});
