# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rails db:seed command (or created alongside the database with db:setup).

# Examples:

#   movies = Movie.create([{ name: 'Star Wars' }, { name: 'Lord of the Rings' }])
#   Character.create(name: 'Luke', movie: movies.first)
require 'faker'

# p "Destroying data....."
# User.destroy_all
# p 'Creating users....'
# 10.times do
#   user = User.new(
#     first_name:  Faker::Name.first_name,
#     last_name:  Faker::Name.last_name,
#     age:  rand(16...80),
#     gender:    ['female', 'male'].sample,
#     email:    Faker::Internet.email,
#     password: Faker::Internet.password(8),
#     address:  Faker::Address.full_address
#     )
#   user.save!
# end


p "Destroying data....."
Expertise.destroy_all
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

# p "Destroying data....."
# Trainer.destroy_all
# p 'creating training_services'

# current_user_id = 0
# current_expertise_id = 0
# 5.times do
#   current_user_id = current_user_id + 1
#   current_expertise_id = current_expertise_id +1
#   trainer = Trainer.new(
#     user_id:   current_user_id,
#     expertise_id:    current_expertise_id,
#     hourly_rate:   rand(100...300)
#     )
#   trainer.save!
# end

