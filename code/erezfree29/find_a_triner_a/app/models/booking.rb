class Booking < ApplicationRecord


  has_one :review
  validates :hours, numericality: { greater_than_or_equal_to: 1 }
end


