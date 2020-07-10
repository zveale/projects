# Embedding Python in a C++ project with Visual Studio: https://devblogs.microsoft.com/python/embedding-python-in-a-cpp-project-with-visual-studio/

# Excute a list of commands for the enemy to follow
from Commands import Enemy

# Commands match enemy commands embedded in EmbeddedModules.h
class Command :
    def execute(self, enemy, dt):
        CommandFunction = getattr(enemy, self.name)
        CommandFunction(dt)
    
class StandToCrawl(Command) :
    name = "StandToCrawl"

class CrawlToStand(Command) :
    name = "CrawlToStand"

class MoveForward(Command) :
    name = "MoveForward"

class TurnRight180(Command) :
    name = "TurnRight180"

class PathfindToPlayer(Command) :
    name = "PathfindToPlayer"

# name of command : function to call
availableCommands = {
   "StandToCrawl" : StandToCrawl(),
   "CrawlToStand" : CrawlToStand(),
   "MoveForward" : MoveForward(),
   "TurnRight180" : TurnRight180(),
   "PathfindToPlayer" : PathfindToPlayer()
    }

# name of command : duration in seconds
script = [
    ("StandToCrawl", 1.25),
    ("MoveForward", 2.0),
    ("CrawlToStand", 1.25),
    ("MoveForward", 2.0),
    ("TurnRight180", 4.0),
    ]

#script = [
#    ("MoveForward", 4.0),
#    ("TurnRight180", 4.0),
#    ]

# Execute current command
def UpdateCommand(enemy, dt) :
    currentDuration = getattr(enemy, "currentDuration")
    currentDuration += dt;

    commandIndex = getattr(enemy, "commandIndex")
    (name, maxDuration) = script[commandIndex]

    if maxDuration < currentDuration :
        commandIndex = (commandIndex + 1) % len(script)
        (name, maxDuration) = script[commandIndex]
        currentDuration = 0.0

    setattr(enemy, "currentDuration", currentDuration)
    setattr(enemy, "commandIndex", commandIndex)

    availableCommands[name].execute(enemy, dt)