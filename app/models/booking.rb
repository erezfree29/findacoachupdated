class Booking < ApplicationRecord

  belongs_to :trainer
  belongs_to :user
  has_one :review
end
