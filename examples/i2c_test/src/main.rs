
use rppal::i2c::I2c;
use std::error::Error;
use std::time::Duration;
use std::thread;
use rppal::gpio::Gpio;
use rppal::gpio::{OutputPin};

#[allow(dead_code)]
fn print_band(lsb : u16, msb : u16) {
	let strobe : u16 = msb >> 4 & 0x07;
	let val : u16 = (msb & 0x03) << 8 | lsb;
	let mut bar : String = String::new();
	let cnt : u8 = (val as f32 / 1024.0 * 12.0) as u8;
	for _i in 0..cnt { bar.push('*'); }
	println!("{}:{}", strobe, bar);
} 

#[allow(dead_code)]
fn print_byte(lsb : u16, msb : u16) {
	println!("{:08b} : {:08b}", msb, lsb);
}

#[allow(dead_code)]
fn print_incoming(buf : Vec<u8>) {
	for chunk in buf.chunks(2) {
		if let [lsb, msb] = chunk {
			print_band(*lsb as u16, *msb as u16);
		} else {
			println!("orphaned byte : {}", chunk[0]);
		}
	}
}

#[allow(dead_code)]
fn reset_tiny85() -> Result<(), Box<dyn Error>> {
	let hun_ms : Duration = Duration::from_millis(100);
	let gpio : Gpio = Gpio::new()?;
	let mut pin : OutputPin = gpio.get(23)?.into_output();
	pin.set_low();
	thread::sleep(hun_ms);
	pin.set_high();
	thread::sleep(hun_ms);
	Ok(())
}

fn main() -> Result<(), Box<dyn Error>> {

	// Join the i2c bus
	let mut i2c : I2c = I2c::new()?;
	i2c.set_slave_address(0x08)?;
	i2c.set_timeout(20)?;

	// reset the peripheral
	reset_tiny85()?;

	{   
		// print all seven filters
		let mut buffer : [u8;14] = [0u8; 14];
		let cmd : [u8;1] = [0xF0];
		i2c.write_read(&cmd, &mut buffer)?;
		print_incoming(buffer.to_vec());
	}

	{   
		// jump to multiplexor pos 3
		let cmd : [u8;1] = [0xC3];
		i2c.write(&cmd)?;
	}

	for _ in 0.. 21 {
		// print 20 consecutive filters
		let mut buffer : [u8;2] = [0u8; 2];
		i2c.read(&mut buffer)?;
		print_incoming(buffer.to_vec());
	}

	Ok(())
	
}
