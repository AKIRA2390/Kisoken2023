import asyncio
import sys
from bleak import BleakClient

notifier_ID = '0000ccdd-0000-1000-8000-00805f9b34fb'

def notifier_callback(handle, data):
  print(handle,"is", data)
  print(type(handle))
  print(type(data))

async def main(address):
  async with BleakClient(address) as client:
    if (not client.is_connected):
      raise "client not connected"

    # services = await client.get_services()


    await client.start_notify(notifier_ID, notifier_callback)
    await asyncio.sleep(60)
    await client.stop_notify(notifier_ID)



if __name__ == "__main__":
  address = 'D4:D4:DA:5C:A1:E6'  

  print('address:', address)
  asyncio.run(main(address))

