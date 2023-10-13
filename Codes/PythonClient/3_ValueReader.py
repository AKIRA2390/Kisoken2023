# 0000ccdd-0000-1000-8000-00805f9b34fb


import asyncio
import sys
from bleak import BleakClient

FIRST_NAME_ID = '00000000-0000-1000-8000-00805f9b34fb'

async def main(address):
  async with BleakClient(address) as client:
    if (not client.is_connected):
      raise "client not connected"

    services = await client.get_services()

    name_bytes = await client.read_gatt_char(FIRST_NAME_ID)
    name = bytearray.decode(name_bytes)
    print('name', name)

if __name__ == "__main__":
  address = 'D4:D4:DA:5C:A4:86'  # Replace this with the actual device address

  print('address:', address)
  asyncio.run(main(address))





