

# p "Destroying data....."

# Trainer.destroy_all



# Booking.destroy_all


# Expertise.destroy_all


# User.destroy_all



p 'Creating expertises'


expertise = Expertise.new(name: "life Coaching")
expertise.save!
expertise = Expertise.new(name: "Health coaching")
expertise.save!
expertise = Expertise.new(name: "Relationship coaching")
expertise.save!
expertise = Expertise.new(name: "Family coaching")
expertise.save!
expertise = Expertise.new(name: "Communication coaching")
expertise.save!
expertise = Expertise.new(name: "Image coaching")
expertise.save!
expertise = Expertise.new(name: "Image coaching")
expertise.save!
expertise = Expertise.new(name: "Retirement coaching")
expertise.save!
expertise = Expertise.new(name: "Career coaching")
expertise.save!
expertise = Expertise.new(name: "Business coaching")
expertise.save!
expertise = Expertise.new(name: "Leadership coaching")
expertise.save!






