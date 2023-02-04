void displayMovementCommand(struct MovementCommand command){
  Serial.print(" X = ");
  Serial.print(command.x);

  Serial.print(" Y = ");
  Serial.print(command.y);

  Serial.print(" Speed = ");
  Serial.print(command.speed);
  Serial.print(" ");  
}
