class BookingsController < ApplicationController
  def index
   @booking = Booking.all
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

    @booking.total_price = params[:booking][:hours].to_i * @trainer.hourly_rate
    @booking.user_id = @user.id
    @booking.trainer_id = @trainer.id

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

params.require(:booking).permit(:hours,:time_of_booking)

end



end


