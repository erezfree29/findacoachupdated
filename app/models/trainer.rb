class Trainer < ApplicationRecord
  belongs_to :user
  # has_many :users, through: :bookings
  # belongs_to :expertises
  has_many :reviews, through: :bookings
  has_many :bookings
end
