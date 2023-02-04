struct MovementCommand getMovementCommandFromJoystick(int x, int y, int speed){
  struct MovementCommand movement;
  movement.x = x;
  movement.y = y;
  movement.speed = speed;
  return movement;
}

int clampMovementCommandComponent(int component, int minimumValue){
  if(component >= 0){
    if(component < minimumValue){
      return 0;
    } else {
      return component;
    }

  } else {
    if(component > minimumValue * -1){
      return 0;
    } else {
      return component;
    }
  }
}

struct MovementCommand filterMovementCommandFromJoystick(struct MovementCommand originalCommand, int minimumValue){
  struct MovementCommand filteredCommand;
  filteredCommand.x = clampMovementCommandComponent(originalCommand.x, minimumValue);
  filteredCommand.y = clampMovementCommandComponent(originalCommand.y, minimumValue);
  filteredCommand.speed = clampMovementCommandComponent(originalCommand.speed, minimumValue);
  return filteredCommand;
}
