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
  console.log('incoming: xid 0x' + xid.toString('hex') + ', chaddr ' + chaddr);

  // check options
  for (var i = 0, len = options.length; i < len;) {
    var kind = options[i];
    if (kind == 0) {
      ++i;
      continue;
    } else if (kind == 255) {
      break;
    }

    var len = options[++i];
    console.log('option kind 0x' + kind.toString(16) + ' length ' + len);
    i += len;
  }
});
