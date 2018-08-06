

# p "Destroying data....."

# Trainer.destroy_all



# Booking.destroy_all


# Expertise.destroy_all


# User.destroy_all



p 'Creating expertises'


expertise = Expertise.new(name: "weight loss")
expertise.save!
expertise = Expertise.new(name: "cardio")
expertise.save!
expertise = Expertise.new(name: "aerobics")
expertise.save!
expertise = Expertise.new(name: "physical recovery")
expertise.save!
expertise = Expertise.new(name: "swimming")
expertise.save!
expertise = Expertise.new(name: "posture")
expertise.save!






