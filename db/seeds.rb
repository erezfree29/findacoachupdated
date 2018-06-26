

p "Destroying data....."

Trainer.destroy_all



Booking.destroy_all


Expertise.destroy_all


User.destroy_all







#p 'Creating users....'
10.times do
  user = User.new(
    first_name:  Faker::Name.first_name,
    last_name:  Faker::Name.last_name,
    age:  rand(16...80),
    gender:    ['female', 'male'].sample,
    email:    Faker::Internet.email,
    password: Faker::Internet.password(8),
    address:  Faker::Address.full_address
    )
  user.save!
end



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



p 'creating training services'


trainer1 = Trainer.create(user_id: 1,expertise_id: 3, hourly_rate: 100,


first_name: "Adi", last_name: "mommy", age: 21, gender: "female",

address:"best place in tel aviv",photo:"image/upload/v1528889148/uclxkxou0gwwgurcbemo.jpg")


trainer2 =Trainer.create(user_id: 1,expertise_id: 3, hourly_rate: 90,


first_name: "albert", last_name: "einstein", age: 120, gender: "female",

address: "32 ben yeuda street tel aviv",photo:"image/upload/v1528889503/vxfmstmh6nxz5viuazto.jpg")


trainer3 =Trainer.create(user_id: 1,expertise_id: 4, hourly_rate: 12,

first_name: "Elton", last_name: "John", age: 120, gender: "male",

address: "beach of La Concha San Sebastián",photo:"image/upload/v1528889741/eh4eftmayrwss4nkphus.jpg")

trainer4 =Trainer.create(user_id: 1,expertise_id: 4, hourly_rate: 12,

first_name: "Aarav", last_name: "Lamba", age: 14, gender: "female",

address: "india Mumbai 1020 buddha street",photo:"image/upload/v1528890155/i6xjauhylfez20tzbfha.jpg")

trainer5 =Trainer.create(user_id: 1,expertise_id: 6, hourly_rate: 60,

first_name: "Michel", last_name: "Jackson", age: 14, gender: "female",

address: "india Mumbai 1020 buddha street",photo:"image/upload/v1528890506/bd6ahgwwik6dlpsxnaxe.jpg",)

trainer6 =Trainer.create(user_id: 1,expertise_id: 6, hourly_rate: 88,

first_name: "Birdigit", last_name: "welsh", age: 14, gender: "female",

address: "Sunny England",photo:"image/upload/v1528890775/be5q2fsuxjka01uqygpe.jpg")

trainer7=Trainer.create(user_id: 1,expertise_id: 6, hourly_rate: 25,

first_name: "Sara", last_name: "The mighty", age: 14, gender: "female",

address: "Sunny England", photo:"image/upload/v1528891161/fgw3rfz9o5shsogtfowu.jpg")

trainer8=Trainer.create(user_id: 1,expertise_id: 4, hourly_rate: 20,

first_name: "John", last_name: "Lennon", age: 14, gender: "female",

address: "Sunny England", photo:"image/upload/v1528922464/usm9xvfqpr2x10o8yovw.jpg")



