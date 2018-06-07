class BookingsController < ApplicationController
  def index
   @booking = Booking.all
  end

  def show
  end

  def new

    @trainer = Trainer.find(params[:trainer_id])
    @booking = Booking.new()
  end

  def create

    @user = current_user
    @trainer = Trainer.find(params[:trainer_id])
    @booking = Booking.new(booking_params)

    @booking.total_price = params[:booking][:hours].to_i * @trainer.hourly_rate
    @booking.user_id = @user
    @booking.trainer_id =@trainer
    @booking.save

  end

  def edit
  end

  def update
  end

  def destroy
end

private
  def booking_params

params.require(:booking).permit(:hours)

end



end
