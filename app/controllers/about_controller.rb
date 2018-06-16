class AboutController < ApplicationController
  skip_before_action :authenticate_user!
  def index
  end

  def show
   @booking = Booking.all
   @user_bookings = []
   @bookings_trainers = []
   @trainers_expertises = []
   counter = 0
   @booking.each do |booking|
    if booking.user_id == current_user.id
      @user_bookings[counter] = booking
      @bookings_trainers[counter] = Trainer.where(id: booking.trainer_id).first
      @trainers_expertises[counter] = Expertise.where(id: @bookings_trainers[counter].expertise_id).first
      counter += 1
    end
  end
 end
end

