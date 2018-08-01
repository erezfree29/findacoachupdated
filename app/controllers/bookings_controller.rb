class BookingsController < ApplicationController


  def index
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
def show

  @booking = Booking.find(params[:id])
  @trainer = Trainer.where(id: @booking.trainer_id).first
  @expertise = Expertise.where(id: @trainer.expertise_id).first
end

def new
  @trainer = Trainer.find(params[:trainer_id])
  @booking = Booking.new

end

def create
  @user = current_user
  @trainer = Trainer.find(params[:trainer_id])
  @booking = Booking.new(booking_params)

  if @booking.date == nil
     flash.now[:alert]  ='please enter the booking date'
   end



  @booking.total_price = params[:booking][:hours].to_i * @trainer.hourly_rate
   @booking.user_id = @user.id
   @booking.trainer_id = @trainer.id
   flag = 0

Booking.all.each do |booking|

if (booking.trainer_id == @booking.trainer_id) && (booking.date == @booking.date)

pre_booking_beg = booking.time.hour * 60 + booking.time.min
pre_booking_end = pre_booking_beg + booking.hours * 60
cur_booking_beg = @booking.time.hour * 60 + @booking.time.min
cur_booking_end = cur_booking_beg + @booking.hours * 60

# current booking can not be between previous booking begining and end
#    #current booking can not end between previous booking begining and end

if (cur_booking_beg >= pre_booking_beg && cur_booking_beg <= pre_booking_end) || (cur_booking_end >= pre_booking_beg && cur_booking_end <= pre_booking_end)

 @booking.user_id =nil
  flash.now[:alert] = 'sorry the coach is not avilable on that time pleae try again'

end

end

end

  if @booking.save
   redirect_to trainer_booking_path(@trainer, @booking)
  else
    render :new

  end
end

def edit
end

def update
end

def destroy
end

private
def booking_params

  params.require(:booking).permit(:hours,:date,:time)

end


end

