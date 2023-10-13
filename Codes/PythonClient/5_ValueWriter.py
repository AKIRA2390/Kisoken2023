# 0000ccdd-0000-1000-8000-00805f9b34fb


import asyncio
from bleak import BleakClient

FIRST_NAME_ID = '00000000-0000-1000-8000-00805f9b34fb'


async def main(address):
  async with BleakClient(address) as client:
    if (not client.is_connected):
      raise "client not connected"
    
    text_to_send = "Hello, BLE Server!"  # 送信する文字列
    
    # 文字列をバイト列に変換
    data_to_write = text_to_send.encode('utf-8')  # UTF-8エンコーディングを使用してバイト列に変換
    
    # データを書き込む
    await client.write_gatt_char(FIRST_NAME_ID, data_to_write , response=True)
    
    # 書き込み後の処理をここに追加できます（例：データの確認など）
    
    # 接続を閉じる
    await client.disconnect()

if __name__ == "__main__":
  address = 'D4:D4:DA:5C:A4:86'  # Replace this with the actual device address

  print('address:', address)
  asyncio.run(main(address))
  






















 