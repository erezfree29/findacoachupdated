# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rails db:seed command (or created alongside the database with db:setup).

# Examples:

#   movies = Movie.create([{ name: 'Star Wars' }, { name: 'Lord of the Rings' }])
#   Character.create(name: 'Luke', movie: movies.first)
# require 'faker'

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


# p "Destroying data....."
# Expertise.destroy_all
# p 'Creating expertises'


# expertise = Expertise.new(name: "weight loss")
# expertise.save!
# expertise = Expertise.new(name: "cardio")
# expertise.save!
# expertise = Expertise.new(name: "aerobics")
# expertise.save!
# expertise = Expertise.new(name: "physical recovery")
# expertise.save!
# expertise = Expertise.new(name: "swimming")
# expertise.save!
# expertise = Expertise.new(name: "posture")
# expertise.save!


p 'creating training_services'



  trainer1 = Trainer.new(first_name: "david")

    # expertise_id: 3
    # hourly_rate: 100
    # first_name: "Adi"
    # last_name: "momy"
    # age: 21
    # gender: "female"
    # address: "best place in tel aviv"



 # trainer1.image = File.open("adi.jpeg")
 # trainer1.save!















# => [#<Trainer:0x00007f2ff4b20068
#   id: 1,
#   hourly_rate: 100,
#   user_id: 1,
#   expertise_id: 3,
#   created_at: Wed, 13 Jun 2018 11:25:46 UTC +00:00,
#   updated_at: Wed, 13 Jun 2018 11:25:46 UTC +00:00,
#   address: "best place in tel aviv",
#   photo: "image/upload/v1528889148/uclxkxou0gwwgurcbemo.jpg",
#   first_name: "Adi ",
#   last_name: "momy ",
#   age: 21,
#   gender: "male">,
#  #<Trainer:0x00007f2ff4b0e1d8
#   id: 2,
#   hourly_rate: 90,
#   user_id: 2,
#   expertise_id: 3,
#   created_at: Wed, 13 Jun 2018 11:31:42 UTC +00:00,
#   updated_at: Wed, 13 Jun 2018 11:31:42 UTC +00:00,
#   address: "32 ben yeuda street tel aviv ",
#   photo: "image/upload/v1528889503/vxfmstmh6nxz5viuazto.jpg",
#   first_name: "albert ",
#   last_name: "einstein",
#   age: 120,
#   gender: "male">,
#  #<Trainer:0x00007f2ff4b0d850
#   id: 3,
#   hourly_rate: 12,
#   user_id: 3,
#   expertise_id: 4,
#   created_at: Wed, 13 Jun 2018 11:35:40 UTC +00:00,
#   updated_at: Wed, 13 Jun 2018 11:35:40 UTC +00:00,
#   address: "beach of La Concha San Sebastián",
#   photo: "image/upload/v1528889741/eh4eftmayrwss4nkphus.jpg",
#   first_name: "Elton",
#   last_name: "Jhon",
#   age: 36,
#   gender: "male">,
#  #<Trainer:0x00007f2ff4b0cc48
#   id: 4,
#   hourly_rate: 10,
#   user_id: 4,
#   expertise_id: 6,
#   created_at: Wed, 13 Jun 2018 11:42:34 UTC +00:00,
#   updated_at: Wed, 13 Jun 2018 11:42:34 UTC +00:00,
#   address: "india Mumbai 1020 buddha street ",
#   photo: "image/upload/v1528890155/i6xjauhylfez20tzbfha.jpg",
#   first_name: "Aarav",
#   last_name: "Lamba",
#   age: 14,
#   gender: "female">,
#  #<Trainer:0x00007f2ff4b0c6d0
#   id: 5,
#   hourly_rate: 230,
#   user_id: 5,
#   expertise_id: 1,
#   created_at: Wed, 13 Jun 2018 11:48:25 UTC +00:00,
#   updated_at: Wed, 13 Jun 2018 11:48:25 UTC +00:00,
#   address: "central park,New york,New York",
#   photo: "image/upload/v1528890506/bd6ahgwwik6dlpsxnaxe.jpg",
#   first_name: "Michel ",
#   last_name: "Jackson",
#   age: 17,
#   gender: "female">,
#  #<Trainer:0x00007f2ff4b0c248
#   id: 7,
#   hourly_rate: 700,
#   user_id: 7,
#   expertise_id: 4,
#   created_at: Wed, 13 Jun 2018 11:52:53 UTC +00:00,
#   updated_at: Wed, 13 Jun 2018 11:52:53 UTC +00:00,
#   address: "Sunny England",
#   photo: "image/upload/v1528890775/be5q2fsuxjka01uqygpe.jpg",
#   first_name: "Birdigit ",
#   last_name: "bridgit ",
#   age: 15,
#   gender: "female">,
#  #<Trainer:0x00007f2ff4b07e50
#   id: 8,
#   hourly_rate: 6,
#   user_id: 8,
#   expertise_id: 3,
#   created_at: Wed, 13 Jun 2018 11:59:20 UTC +00:00,
#   updated_at: Wed, 13 Jun 2018 11:59:20 UTC +00:00,
#   address: "not your business",
#   photo: "image/upload/v1528891161/fgw3rfz9o5shsogtfowu.jpg",
#   first_name: "Sara",
#   last_name: "The mighty",
#   age: 12,
#   gender: "female">,
#  #<Trainer:0x00007f2ff4b07900
#   id: 11,
#   hourly_rate: 40,
#   user_id: 9,
#   expertise_id: 4,
#   created_at: Wed, 13 Jun 2018 20:41:03 UTC +00:00,
#   updated_at: Wed, 13 Jun 2018 20:41:03 UTC +00:00,
#   address: "sixty nine jumpt street ",
#   photo: "image/upload/v1528922464/usm9xvfqpr2x10o8yovw.jpg",
#   first_name: "Jhon",
#   last_name: "Lennon",
#   age: 70,
#   gender: "male">]
