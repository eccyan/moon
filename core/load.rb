require './core/state.rb'
require './core/mouse.rb'

class State_Test < State

  def initialize(engine)
    super(engine)
    @sprites = []

    #@sound = Sound.new("resources/startup.wav", "wav")
    #@sound.play
    Music.play("resources/st01no01m_theme.ogg", "ogg")
    @s = Spritesheet.new("resources/hyptosis_tile-art-batch-1.png", 32, 32)
    # 6 FPS at 15000 items
    # 55 FPS at 1500 items
    #1500.times do
      #sprite = Sprite.new("resources/obama_sprite.png")
      #sprite.x = rand(640)
      #sprite.y = rand(480)
      #@sprites << sprite
    #end
  end

  def update
    p "SPACE is PRESSED!" if Input.pressed?(Input::Keys::SPACE)
    p "Mouse is in area!" if Input::Mouse.in_area?(0, 0, 32, 32)
    p Input::Mouse.pos
    for i in 0..1800 # runs efficiently at 60FPS, 18000 runs at 20FPS
      @s.render(i, i, i)
    end
    #@sprites.each {|sprite| sprite.render }
  end

end

State.push(State_Test)