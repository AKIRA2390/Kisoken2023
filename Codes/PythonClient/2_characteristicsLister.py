import asyncio
import sys
from bleak import BleakClient

async def main(address):
  async with BleakClient(address) as client:
    if (not client.is_connected):
      raise "client not connected"

    services = await client.get_services()

    for service in services:
      print('\nservice', service.handle, service.uuid, service.description)

      characteristics = service.characteristics

      for char in characteristics:
        print('  characteristic', char.handle, char.uuid, char.description, char.properties)

        descriptors = char.descriptors

        for desc in descriptors:
          print('    descriptor', desc)

if __name__ == "__main__":
  address =  'D4:D4:DA:5C:A4:86'  # Replace this with the actual device address

  print('address:', address)
  asyncio.run(main(address))







  