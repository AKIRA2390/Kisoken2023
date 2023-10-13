# Dobot Studio で実行するプログラム
# - Python 3.5 なので f文字列は使用できない

import socket
import logging
import time
import json

# Dobot Studio で実行する場合は True に設定
DOBOT_STUDIO_ENV = False

logging.basicConfig(format='[%(levelname)s] %(asctime)s: %(message)s')
log = logging.getLogger(__name__)
log.setLevel(level=logging.DEBUG)

#HOST = '192.168.33.40'
HOST = '127.0.0.1'
PORT = 8893
BUFFER_SIZE = 1024

REACHEDSTATS = "OK"
DistX=0
DistY=0
DistZ=0
DistR=0

def exec_cmd(c):

  if 'command' not in c :
    msg= 'KeyError : Not found "command" key.'
    log.error(msg)
    return dict(status='Error',msg=msg)

  try :

    # 基本的には、ここのelif節の実装を拡張していく。

    #JumpTo命令 
    if c['command'] == 'JumpTo' :
      log.info('JumpTo ({0},{1},{2},{3})'.format(c['A'],c['B'],c['C'],c['D']))
      if DOBOT_STUDIO_ENV :
        dType.SetPTPCmdEx(api,0,c['A'],c['B'],c['C'],c['D'],1)
      res = dict(is_sccess=True)

    #JumpJointTo命令 
    elif c['command'] == 'JumpJointTo' :
      log.info('JumpJointTo ({0},{1},{2},{3})'.format(c['A'],c['B'],c['C'],c['D']))
      if DOBOT_STUDIO_ENV :
        dType.SetPTPCmdEx(api,3,c['A'],c['B'],c['C'],c['D'],1)
      res = dict(is_sccess=True)

    #GoTo命令 
    elif c['command'] == 'GoTo' :
      log.info('GoTo ({0},{1},{2},{3})'.format(c['A'],c['B'],c['C'],c['D']))
      if DOBOT_STUDIO_ENV :
        dType.SetPTPCmdEx(api,2,c['A'],c['B'],c['C'],c['D'],1)
      res = dict(is_sccess=True)

    #Wait命令 
    elif c['command'] == 'Wait' :
      log.info('Wait ({0} ms)'.format(c['A']))
      if DOBOT_STUDIO_ENV :
        dType.SetWAITCmdEx(api,c['A'],1)
      res = dict(is_sccess=True)

    #SetOutput命令 
    elif c['command'] == 'SetOutput' :
      log.info('SetOutput ({0} pin / Value {1})'.format(c['A'],c['B']))
      if DOBOT_STUDIO_ENV :
        dType.SetIODOEx(api, c['A'], c['B'], 1)
      res = dict(is_sccess=True)

    #Get(Analog)Input命令
    elif c['command'] == 'GetInput' :
      if DOBOT_STUDIO_ENV :
        result=dType.GetIODI(api,c["A"])[0]
      else :
        result=334 # Dummy値
      log.info('GetInput ({0} pin / Value {1})'.format(c['A'],result))
      res = dict(status='OK',value=result)

    #ArmOrientation命令
    elif c['command'] == 'ArmOrientation' :
      log.info('ArmOrientation ({0} mode)'.format(c['A']))
      if DOBOT_STUDIO_ENV :
        dType.SetArmOrientationEx(api,c['A'], 1)
      res = dict(is_sccess=True)

    #SetCordinateSpeed命令 
    elif c['command'] == 'SetCordinateSpeed' :
      log.info('SetCordinateSpeed ({0} {1})'.format(c['A'],c['B']))
      if DOBOT_STUDIO_ENV :
        dType.SetPTPCommonParamsEx(api,c['A'],c['B'],1)
      res = dict(is_sccess=True)

    #SetJumpPram命令 
    elif c['command'] == 'SetJumpPram' :
      log.info('SetJumpPram (hieght = {0} zlimit = {1})'.format(c['A'],c['B']))
      if DOBOT_STUDIO_ENV :
        dType.SetPTPJumpParamsEx(api,c['A'],c['B'],1)
      res = dict(is_sccess=True)

    elif c['command'] == 'Ping' :
      log.info('Ping')
      res = dict(is_sccess=True)

    elif c['command'] == 'Quit' :
      log.info('Quit')
      res = dict(is_sccess=True)

    elif c['command'] == 'Reached' :
      log.info('Reached')
      res = dict(is_sccess=REACHEDSTATS)

    else :
      msg = 'Unknown commad : {0}'.format(str(c['command']))
      log.error(msg)
      res = dict(is_sccess=False,msg=msg)

  except KeyError:
    cmd_arg = [ s for s in c.keys() if s != 'command']
    msg = 'Invalid argument : "{0}" -> {{{1}}}'.format(c['command'],", ".join(cmd_arg))
    log.error(msg)
    res = dict(is_sccess=False,msg=msg)

  return res


def hasReached(cmd_dict,Pose):
  if(cmd_dict["command"] != "MOVJ"):
    return "YET"
  print(cmd_dict)
  DestX = cmd_dict['A']
  print("inside hasreached")
  DestY = cmd_dict['B']
  print("inside hasreached")
  DestZ = cmd_dict['C']
  print("inside hasreached")
  DestR = cmd_dict['D']
  print("inside hasreached")

  if(abs(Pose[0] - DestX) < 5) and \
    (abs(Pose[1] - DestY) < 5) and \
        (abs(Pose[2] - DestZ) < 5) and\
            (abs(Pose[3] - DestR) < 5):
    log.info('Reached: OK')
    return "OK"
  else:
    log.info('Reached: YET')
    return "YET"
        
  
#### MainLoop ###

try:
  with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    while True :
      cmd_dict = {'command':'JumpTo','A':200,'B':0,'C':80,'D':0}
      c, client = s.accept()
      log.debug('Client connected {0}'.format(client))
      try:  
        cmd_dict = json.loads(c.recv(BUFFER_SIZE).decode('UTF-8'))
        res = exec_cmd(cmd_dict)
        c.send(json.dumps(res,ensure_ascii=False).encode())
      finally:
        c.close()
        if 'command' in cmd_dict :
          if cmd_dict['command']=='Quit' :
            break
      if DOBOT_STUDIO_ENV :
        PoseData = dType.GetPose(api)
        log.info(PoseData)
        print("after loginfo")
        REACHEDSTATS = hasReached(cmd_dict,PoseData)
        print("after hasreached")
    
finally:
  s.close()