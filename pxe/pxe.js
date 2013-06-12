var dgram = require('dgram');

if (!('SUDO_UID' in process.env)) {
  console.log('SUDO_UID not in process.env; don\'t know how to drop privileges');
  process.exit(1);
}

var s = dgram.createSocket('udp4');
s.bind(67, function() {
  process.setuid(parseInt(process.env.SUDO_UID));
});

s.on('message', function(msg, rinfo) {
  var op = msg.readUInt8(0),
      htype = msg.readUInt8(1),
      hlen = msg.readUInt8(2),
      hops = msg.readUInt8(3),
      xid = msg.slice(4, 8),
      magic = msg.readUInt32BE(236);
  var options = msg.slice(240);

  if (magic != 0x63825363) {
    console.log('rejecting bad client; incorrect magic');
    return;
  }

  if (op != 1 || htype != 1 || hlen != 6 || hops != 0) {
    console.log('rejecting bad client: wrong op/htype/hlen/hops');
    return;
  }

  var chaddr = [];
  for (var i = 0; i < hlen; ++i) {
    chaddr.push(msg.slice(28 + i, 28 + i + 1).toString('hex'));
  }
  chaddr = chaddr.join(':')

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

  if (!optdict[97]) {
    console.log('doesn\'t look like pxe; no cmi');
    return;
  }

  console.log('pxe request incoming: xid 0x' + xid.toString('hex') + ', chaddr ' + chaddr);

  // offer some arbitrary IP for PXE

});
