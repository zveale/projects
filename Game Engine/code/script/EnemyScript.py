# Embedding Python in a C++ project with Visual Studio: https://devblogs.microsoft.com/python/embedding-python-in-a-cpp-project-with-visual-studio/

# Excute a list of commands for the enemy to follow
from Commands import Enemy

# Commands match enemy commands embedded in EmbeddedModules.h
class Command:
    def execute(self, enemy, dt):
        CommandFunction = getattr(enemy, self.name)
        CommandFunction(dt)
    
class stand_to_crawl(Command):
    name = "stand_to_crawl"

class crawl_to_stand(Command):
    name = "crawl_to_stand"

class move_forward(Command):
    name = "move_forward"

class turn_right_180(Command):
    name = "turn_right_180"

class pathfind_to_player(Command):
    name = "pathfind_to_player"

# name of command : function to call
availableCommands = {
   "stand_to_crawl" : StandToCrawl(),
   "crawl_to_stand" : CrawlToStand(),
   "move_forward" : MoveForward(),
   "turn_right_180" : TurnRight180(),
   "pathfind_to_player" : PathfindToPlayer()
    }

# name of command : duration in seconds
script = [
    ("stand_to_crawl", 1.25),
    ("move_forward", 2.0),
    ("crawl_to_stand", 1.25),
    ("move_forward", 2.0),
    ("turn_right_180", 4.0),
    ]

#script = [
#    ("move_forward", 4.0),
#    ("turn_right_180", 4.0),
#    ]

# Execute current command
def update_command(enemy, dt) :
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