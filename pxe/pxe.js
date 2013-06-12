var dgram = require('dgram');

var s = dgram.createSocket('udp4');
s.bind(67, function() {
  process.setuid(1000);
});

s.on('message', function(msg, rinfo) {
  var op = msg.readUInt8(0),
      htype = msg.readUInt8(1),
      hlen = msg.readUInt8(2),
      hops = msg.readUInt8(3),
      xid = msg.readUInt32BE(4),
      chaddr1 = msg.readUInt32BE(28),
      chaddr2 = msg.readUInt32BE(32),
      chaddr3 = msg.readUInt32BE(36),
      chaddr4 = msg.readUInt32BE(40),
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
  console.log('xid ' + xid +
    ', chaddr ' + chaddr +
    ', options ' + options.toString('hex'));
});
