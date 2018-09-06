class Review < ApplicationRecord
  belongs_to :booking
  has_one  :trainer, through: :bookings
  has_many :reviews, through: :bookings
  validates :content,:title, :score ,presence: true
  validates :booking_id, uniqueness: true

end


