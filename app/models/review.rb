class Review < ApplicationRecord
  belongs_to :booking
  has_one  :trainer, through: :bookings
  has_many :reviews, through: :bookings

end
